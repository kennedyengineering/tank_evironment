# Tank Game (@kennedyengineering)

from tank_game_environment import tank_game_environment_v1

from tank_game_agent.callback.callback_video_recorder import VideoRecorderCallback
from tank_game_agent.callback.callback_hparam_recorder import HParamRecorderCallback

from tank_game_agent.schedule.schedule_linear import linear_schedule
from tank_game_agent.schedule.schedule_cosine import cosine_schedule

from tank_game_agent.feature_extactor.feature_extractor_wrapper import (
    FeatureExtractorWrapper,
)

import time
import os
import argparse
import numpy as np

from moviepy.video.io.ImageSequenceClip import ImageSequenceClip

from stable_baselines3 import PPO
from stable_baselines3.common.evaluation import evaluate_policy
from stable_baselines3.common.env_util import make_vec_env
from stable_baselines3.common.callbacks import (
    CallbackList,
    CheckpointCallback,
    EvalCallback,
)
from stable_baselines3.common.vec_env import DummyVecEnv

from sb3_contrib import RecurrentPPO
from sb3_contrib.ppo_recurrent.policies import MlpLstmPolicy

import torch


def train(checkpoint_path, map_name, feature_model_path):
    """Train an agent."""

    # Configuration variables
    num_envs = 12
    num_eval_episodes = 10
    steps = 12_000_000
    seed = 0  # if continuing from a checkpoint might want to specify a different seed
    device = "cpu"
    log_dir = "logs/"
    save_dir = "weights/"
    save_freq = 100_000
    eval_freq = 10_000
    verbose = 3
    schedule_learning_rate = True
    schedule_clip_range = False
    policy_kwargs = dict(
        # n_lstm_layers=2,
        # lstm_hidden_size=512,
        # shared_lstm=True,
        # enable_critic_lstm=False,
        log_std_init=-2.0,
    )
    env_kwargs = dict(
        scripted_policy_name="StaticAgent",
        scripted_policy_kwargs=dict(action=[0.0, 0.0, 0.0]),
        map_id=map_name,
    )

    feature_model = PPO.load(
        feature_model_path,
        device=device,
        seed=seed,
    )
    feature_model.policy.features_extractor.eval()

    env_kwargs["wrappers"] = [FeatureExtractorWrapper]
    env_kwargs["wrappers_kwargs"] = {
        FeatureExtractorWrapper: dict(
            feature_extractor=feature_model.policy.features_extractor
        )
    }

    # PPO configuration variables
    ppo_config = {
        "learning_rate": 3e-4,
        "n_steps": 512,
        "batch_size": 512,
        "n_epochs": 5,
        "gamma": 0.99,
        "gae_lambda": 0.95,
        "clip_range": 0.2,
        "ent_coef": 0.005,
        "vf_coef": 0.5,
        "max_grad_norm": 0.5,
    }

    torch.set_num_threads(6)

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
        model = RecurrentPPO(
            policy=MlpLstmPolicy,
            env=env,
            verbose=verbose,
            device=device,
            tensorboard_log=log_dir,
            seed=seed,
            policy_kwargs=policy_kwargs,
            **ppo_config,
        )
    else:
        model = RecurrentPPO.load(
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
    model_path, map_name, feature_model_path, record_video, num_episodes, deterministic
):
    """Evaluate an agent."""

    if record_video is not None:
        print(f"Recording video, saving to {record_video}")

    # Configuration variables
    seed = 100
    device = "cpu"
    env_kwargs = dict(
        scripted_policy_name="StaticAgent",
        scripted_policy_kwargs=dict(
            action=[1.0, 1.0, 0.0] if "Run" in map_name else [0.0, 0.0, 0.0]
        ),
        map_id=map_name,
        render_mode="rgb_array" if record_video else "human",
    )

    feature_model = PPO.load(
        feature_model_path,
        device=device,
        seed=seed,
    )
    feature_model.policy.features_extractor.eval()

    env_kwargs["wrappers"] = [FeatureExtractorWrapper]
    env_kwargs["wrappers_kwargs"] = {
        FeatureExtractorWrapper: dict(
            feature_extractor=feature_model.policy.features_extractor
        )
    }

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
    model = RecurrentPPO.load(model_path, device=device, seed=seed)

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
    callbacks = [log_win_stats_callback, log_frames_callback]
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
    train_parser.add_argument(
        "--feature-model",
        type=str,
        default="weights/tank_game_environment_v1_20250417-053308/tank_game_environment_v1_20250417-053308.zip",
        help="Path to the model with the feature extractor checkpoint.",
    )

    # Evaluation mode
    eval_parser = subparsers.add_parser("eval", help="Run model evaluation.")
    eval_parser.add_argument("model_path", type=str, help="Path to the trained model.")
    eval_parser.add_argument(
        "--map", type=str, default="Random", help="Name of the map."
    )
    eval_parser.add_argument(
        "--feature-model",
        type=str,
        default="weights/tank_game_environment_v1_20250417-053308/tank_game_environment_v1_20250417-053308.zip",
        help="Path to the model with the feature extractor checkpoint.",
    )
    eval_parser.add_argument(
        "--record-video",
        type=str,
        default=None,
        help="Path to output video file (e.g. output.mp4).",
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
        train(args.model_path, args.map, args.feature_model)
    elif args.mode == "eval":
        print("Evaluation mode selected.")
        eval(
            args.model_path,
            args.map,
            args.feature_model,
            args.record_video,
            args.episodes,
            not args.stochastic,
        )
