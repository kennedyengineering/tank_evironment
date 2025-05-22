# Tank Game (@kennedyengineering)

from tank_game_environment import tank_game_environment_v1

from tank_game_agent.callback.callback_video_recorder import VideoRecorderCallback
from tank_game_agent.callback.callback_hparam_recorder import HParamRecorderCallback

from tank_game_agent.schedule.schedule_linear import linear_schedule
from tank_game_agent.schedule.schedule_cosine import cosine_schedule

from tank_game_agent.feature_extactor.feature_extractor_lidar import LidarCNN

from tank_game_agent.analysis.plot_actions import plot_actions
from tank_game_agent.analysis.plot_latent import (
    plot_conv,
    plot_latent,
    plot_latent_heatmap,
)
from tank_game_agent.analysis.plot_observations import plot_observations

import time
import os
import argparse
import numpy as np

from moviepy.video.io.ImageSequenceClip import ImageSequenceClip

from stable_baselines3 import PPO
from stable_baselines3.ppo import MlpPolicy
from stable_baselines3.common.evaluation import evaluate_policy
from stable_baselines3.common.env_util import make_vec_env
from stable_baselines3.common.callbacks import (
    CallbackList,
    CheckpointCallback,
    EvalCallback,
)
from stable_baselines3.common.vec_env import DummyVecEnv

import torch


def train(checkpoint_path, map_name):
    """Train an agent."""

    # Configuration variables
    num_envs = 12
    num_eval_episodes = 10
    steps = 18_000_000
    seed = 0  # if continuing from a checkpoint might want to specify a different seed
    device = "cuda"
    log_dir = "logs/"
    save_dir = "weights/"
    save_freq = 100_000
    eval_freq = 10_000
    verbose = 3
    schedule_learning_rate = True
    schedule_clip_range = False
    policy_kwargs = dict(
        features_extractor_class=LidarCNN,
        features_extractor_kwargs=dict(features_dim=256),
        net_arch=dict(pi=[128, 128], vf=[128, 128]),
    )
    env_kwargs = dict(
        scripted_policy_name="StaticAgent",
        scripted_policy_kwargs=dict(action=[0.0, 0.0, 0.0]),
        map_id=map_name,
    )

    # PPO configuration variables
    ppo_config = {
        "learning_rate": 3e-4,
        "n_steps": 1024,
        "batch_size": 256,
        "n_epochs": 10,
        "gamma": 0.99,
        "gae_lambda": 0.95,
        "clip_range": 0.2,
        "ent_coef": 0.01,
        "vf_coef": 0.5,
        "max_grad_norm": 0.5,
    }

    # Create environments
    env = make_vec_env(
        tank_game_environment_v1.env_fn,
        n_envs=num_envs,
        env_kwargs=env_kwargs,
        seed=seed,
        vec_env_cls=DummyVecEnv,
    )

    eval_env = make_vec_env(
        tank_game_environment_v1.env_fn,
        n_envs=num_envs,
        env_kwargs=env_kwargs,
        seed=seed + num_envs,
        vec_env_cls=DummyVecEnv,
    )

    render_env = make_vec_env(
        tank_game_environment_v1.env_fn,
        n_envs=1,
        env_kwargs=env_kwargs | dict(render_mode="rgb_array"),
        seed=seed + 2 * num_envs,
        vec_env_cls=DummyVecEnv,
    )

    env_name = render_env.unwrapped.metadata["name"]
    run_name = f"{env_name}_{time.strftime('%Y%m%d-%H%M%S')}"
    save_dir = os.path.join(save_dir, run_name)

    # Handle scheduling learning rate and clip range
    ppo_config_copy = ppo_config.copy()
    if schedule_learning_rate:
        ppo_config["learning_rate"] = cosine_schedule(ppo_config["learning_rate"])
    if schedule_clip_range:
        ppo_config["clip_range"] = linear_schedule(ppo_config["clip_range"])

    # Create model
    if checkpoint_path is None:
        model = PPO(
            policy=MlpPolicy,
            env=env,
            verbose=verbose,
            device=device,
            tensorboard_log=log_dir,
            seed=seed,
            policy_kwargs=policy_kwargs,
            **ppo_config,
        )
    else:
        model = PPO.load(
            path=checkpoint_path,
            env=env,
            verbose=verbose,
            device=device,
            tensorboard_log=log_dir,
            seed=seed,
            policy_kwargs=policy_kwargs,
            **ppo_config,
        )

    # Setup callbacks
    hparam_callback = HParamRecorderCallback(
        hparam_dict={
            "environment": env_name,
            "seed": seed,
            "n_envs": num_envs,
            "n_eval_episodes": num_eval_episodes,
            "eval_freq": eval_freq,
            "steps": steps,
            "checkpoint_path": str(checkpoint_path),
            "checkpoint_freq": save_freq,
            "device": device,
        }
        | ppo_config_copy,
        metric_dict={"eval/mean_ep_length": 0, "eval/mean_reward": 0},
    )
    checkpoint_callback = CheckpointCallback(
        save_freq=max(save_freq // num_envs, 1),
        save_path=save_dir,
        name_prefix=run_name,
        verbose=verbose,
    )
    video_callback = VideoRecorderCallback(eval_env=render_env, render_freq=1)
    eval_callback = EvalCallback(
        eval_env=eval_env,
        callback_on_new_best=video_callback,
        n_eval_episodes=num_eval_episodes,
        eval_freq=max(eval_freq // num_envs, 1),
        best_model_save_path=save_dir,
        verbose=verbose,
    )
    callbacks = CallbackList([hparam_callback, checkpoint_callback, eval_callback])

    # Train model
    print(f"Starting training on {env_name}. ({run_name})")
    model.learn(
        total_timesteps=steps,
        tb_log_name=run_name,
        callback=callbacks,
        reset_num_timesteps=bool(checkpoint_path is None),
    )
    print(f"Finished training on {env_name}. ({run_name})")

    # Save model
    model.save(os.path.join(save_dir, run_name))
    print("Model has been saved.")

    # Close environment
    env.close()


def eval(
    model_path,
    map_name,
    record_video,
    record_actions,
    record_observations,
    record_features,
    num_episodes,
    deterministic,
):
    """Evaluate an agent."""

    torch.set_num_threads(1)

    if record_video is not None:
        print(f"Recording video, saving to {record_video}")

    if record_actions:
        print(f"Recording actions, saving to {record_actions}")

    if record_observations:
        print(f"Recording observations, saving to {record_observations}")

    if record_features:
        print(f"Recording features, saving to {record_features}")

    # Configuration variables
    seed = 100
    device = "cuda"
    env_kwargs = dict(
        scripted_policy_name="StaticAgent",
        scripted_policy_kwargs=dict(
            action=[1.0, 1.0, 0.0] if "Run" in map_name else [0.0, 0.0, 0.0]
        ),
        map_id=map_name,
        render_mode="rgb_array" if record_video else "human",
    )

    # Create environment
    eval_env = make_vec_env(
        tank_game_environment_v1.env_fn,
        n_envs=1,
        seed=seed,
        env_kwargs=env_kwargs,
        vec_env_cls=DummyVecEnv,
    )

    eval_env_name = eval_env.metadata["name"]

    # Load model
    print(f"Loading model {model_path}.")
    model = PPO.load(model_path, device=device, seed=seed)

    # Log features
    conv1 = []
    conv2 = []
    conv_mlp = []
    extra = []

    def log_features(locals_, _):
        if record_features:

            feature_extractor = model.policy.features_extractor

            observations = torch.from_numpy(locals_["observations"])
            observations = observations.to(device=model.device)

            # Lidar Head.
            x = feature_extractor.forward_conv_preprocess(observations)
            x = feature_extractor.forward_conv_block1(x)
            conv1.append(x.cpu().detach().numpy())
            x = feature_extractor.forward_conv_block2(x)
            conv2.append(x.cpu().detach().numpy())
            cnn_features = feature_extractor.forward_conv_features(x)
            conv_mlp.append(cnn_features.cpu().detach().numpy())

            # Extra features Head.
            x = feature_extractor.forward_extra_preprocess(observations)
            extra_features = feature_extractor.forward_extra_mlp(x)
            extra.append(extra_features.cpu().detach().numpy())

            done = locals_["done"]
            if done:
                # Prepare plot
                np_conv1 = np.squeeze(conv1)
                np_conv2 = np.squeeze(conv2)
                np_conv_mlp = np.squeeze(conv_mlp)
                np_extra = np.squeeze(extra)

                # Plot and save plot to disk
                idx = locals_["episode_counts"][0]
                base, ext = os.path.splitext(record_features)

                filename_conv1 = f"{base}_conv1_{idx}.png"
                filename_conv2 = f"{base}_conv2_{idx}.png"
                filename_conv_mlp = f"{base}_conv_mlp_{idx}.png"
                filename_extra = f"{base}_extra_{idx}.png"

                plot_conv(np_conv1, save_path=filename_conv1)
                plot_conv(np_conv2, save_path=filename_conv2)
                plot_latent_heatmap(np_conv_mlp, save_path=filename_conv_mlp)
                plot_latent(np_extra, save_path=filename_extra)

                filename_conv1 = f"{base}_conv1_{idx}.pdf"
                filename_conv2 = f"{base}_conv2_{idx}.pdf"
                filename_conv_mlp = f"{base}_conv_mlp_{idx}.pdf"
                filename_extra = f"{base}_extra_{idx}.pdf"

                plot_conv(np_conv1, save_path=filename_conv1)
                plot_conv(np_conv2, save_path=filename_conv2)
                plot_latent_heatmap(np_conv_mlp, save_path=filename_conv_mlp)
                plot_latent(np_extra, save_path=filename_extra)

                # Clear buffers
                conv1.clear()
                conv2.clear()
                conv_mlp.clear()
                extra.clear()

    # Log observations
    observations = []

    def log_observations(locals_, _):
        if record_observations:
            observations.append(locals_["observations"])

            done = locals_["done"]
            if done:
                # Prepare plot
                np_observations = np.array(observations)
                np_observations = np.squeeze(observations)

                # Plot and save plot to disk
                idx = locals_["episode_counts"][0]

                base, ext = os.path.splitext(record_observations)
                filename_lidar = f"{base}_lidar_{idx}.png"
                filename_extra = f"{base}_extra_{idx}.png"
                plot_observations(
                    np_observations,
                    lidar_save_path=filename_lidar,
                    extra_save_path=filename_extra,
                )
                filename_lidar = f"{base}_lidar_{idx}.pdf"
                filename_extra = f"{base}_extra_{idx}.pdf"
                plot_observations(
                    np_observations,
                    lidar_save_path=filename_lidar,
                    extra_save_path=filename_extra,
                )

                # Clear buffer
                observations.clear()

    # Log actions
    actions = []

    def log_actions(locals_, _):
        if record_actions:
            actions.append(locals_["actions"])

            done = locals_["done"]
            if done:
                # Prepare plot
                np_actions = np.array(actions)
                np_actions = np.squeeze(actions)

                # Plot and save plot to disk
                idx = locals_["episode_counts"][0]

                base, ext = os.path.splitext(record_actions)
                filename = f"{base}_{idx}.png"
                plot_actions(np_actions, save_path=filename)
                filename = f"{base}_{idx}.pdf"
                plot_actions(np_actions, save_path=filename)

                # Reset buffer
                actions.clear()

    # Log win stats
    win_log = np.zeros(num_episodes)
    tie_log = np.zeros(num_episodes)
    loss_log = np.zeros(num_episodes)
    timeout_log = np.zeros(num_episodes)

    def log_win_stats_callback(locals_, _):
        done = locals_["done"]
        if done:
            infos = locals_["infos"][0]
            idx = locals_["episode_counts"][0]

            won = "hit" in infos and not "hit_by" in infos
            win_log[idx] = int(won)

            tie = "hit" in infos and "hit_by" in infos
            tie_log[idx] = int(tie)

            lost = not "hit" in infos and "hit_by" in infos
            loss_log[idx] = int(lost)

            timeout = not "hit" in infos and not "hit_by" in infos
            timeout_log[idx] = int(timeout)

    # Log frames
    frames = []

    def log_frames_callback(locals_, _):
        if record_video:
            frames.append(locals_["env"].render())

            done = locals_["done"]
            if done:
                # Save video to disk
                idx = locals_["episode_counts"][0]

                base, ext = os.path.splitext(record_video)
                filename = f"{base}_{idx}{ext}"

                clip = ImageSequenceClip(frames, fps=30)
                clip.write_videofile(
                    filename,
                    audio=False,
                    logger=None,
                )

                # Reset frame buffer
                frames.clear()

    # Create callback list
    callbacks = [
        log_features,
        log_observations,
        log_actions,
        log_win_stats_callback,
        log_frames_callback,
    ]
    master_callback = lambda locals_, _: [fn(locals_, _) for fn in callbacks]

    # Run evaluation
    print(f"Starting evaluation on {eval_env_name}. (num_episodes={num_episodes})")
    rewards = evaluate_policy(
        model,
        eval_env,
        n_eval_episodes=num_episodes,
        deterministic=deterministic,
        render=False,
        return_episode_rewards=True,
        callback=master_callback,
    )
    print("Rewards: ", rewards[0])
    print("Episode Durations: ", rewards[1])
    print("Average Reward: ", np.mean(rewards[0]))
    print("Average Duration: ", np.mean(rewards[1]))
    print("Win Log:\t", win_log)
    print("Tie Log:\t", tie_log)
    print("Loss Log:\t", loss_log)
    print("Timeout Log:\t", timeout_log)
    print("Win Rate: ", np.count_nonzero(win_log) / win_log.size)
    print("Tie Rate: ", np.count_nonzero(tie_log) / tie_log.size)
    print("Loss Rate: ", np.count_nonzero(loss_log) / loss_log.size)
    print("Timeout Rate: ", np.count_nonzero(timeout_log) / timeout_log.size)


if __name__ == "__main__":

    parser = argparse.ArgumentParser(description="Train or evaluate a model.")

    # Create subparsers for 'train' and 'eval' modes
    subparsers = parser.add_subparsers(dest="mode", required=True)

    # Training mode
    train_parser = subparsers.add_parser("train", help="Run model training.")
    train_parser.add_argument(
        "model_path", type=str, nargs="?", help="Path to the model checkpoint."
    )
    train_parser.add_argument(
        "--map", type=str, default="Random", help="Name of the map."
    )

    # Evaluation mode
    eval_parser = subparsers.add_parser("eval", help="Run model evaluation.")
    eval_parser.add_argument("model_path", type=str, help="Path to the trained model.")
    eval_parser.add_argument(
        "--map", type=str, default="Random", help="Name of the map."
    )
    eval_parser.add_argument(
        "--record-video",
        type=str,
        default=None,
        help="Path to output video file (e.g. output.mp4).",
    )
    eval_parser.add_argument(
        "--record-actions",
        type=str,
        default=None,
        help="Path to output plot files (e.g. actions.png or actions.pdf).",
    )
    eval_parser.add_argument(
        "--record-observations",
        type=str,
        default=None,
        help="Path to output plot files (e.g. observations.png or observations.pdf).",
    )
    eval_parser.add_argument(
        "--record-features",
        type=str,
        default=None,
        help="Path to output plot files (e.g. features.png or features.pdf).",
    )
    eval_parser.add_argument(
        "--episodes", type=int, default=20, help="Number of episodes to run."
    )
    eval_parser.add_argument(
        "--stochastic",
        action="store_true",
        help="Perform stochastic inference of the agent or not.",
    )

    # Parse arguments
    args = parser.parse_args()

    if args.mode == "train":
        print("Training mode selected.")
        train(args.model_path, args.map)
    elif args.mode == "eval":
        print("Evaluation mode selected.")
        eval(
            args.model_path,
            args.map,
            args.record_video,
            args.record_actions,
            args.record_observations,
            args.record_features,
            args.episodes,
            not args.stochastic,
        )
