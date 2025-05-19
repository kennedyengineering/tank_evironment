# Tank Game (@kennedyengineering)

from tank_game_environment import tank_game_environment_v1

from tank_game_agent.callback.callback_video_recorder import VideoRecorderCallback
from tank_game_agent.callback.callback_hparam_recorder import HParamRecorderCallback

from tank_game_agent.schedule.schedule_linear import linear_schedule
from tank_game_agent.schedule.schedule_cosine import cosine_schedule

from tank_game_agent.feature_extactor.feature_extractor_wrapper import (
    FeatureExtractorWrapper,
)

from tank_game_agent.vec_env.vec_env import TankVecEnv

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

from sb3_contrib import RecurrentPPO
from sb3_contrib.ppo_recurrent.policies import MlpLstmPolicy

import torch


def train(opponent_model_path, checkpoint_path, map_name, feature_model_path):
    """Train an agent."""

    # Configuration variables
    num_envs = 12
    num_eval_episodes = 10
    steps = 6_000_000
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
    )
    vec_env_kwargs = dict(
        opponent_model=PPO.load(opponent_model_path, device=device, seed=seed),
        opponent_predict_deterministic=True,
    )
    env_kwargs = dict(
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
        "ent_coef": 0.01,
        "vf_coef": 0.5,
        "max_grad_norm": 0.5,
    }

    torch.set_num_threads(6)

    # Create environments
    env = make_vec_env(
        tank_game_environment_v1.env_fn,
        n_envs=num_envs,
        seed=seed,
        env_kwargs=env_kwargs,
        vec_env_cls=TankVecEnv,
        vec_env_kwargs=vec_env_kwargs,
    )

    eval_env = make_vec_env(
        tank_game_environment_v1.env_fn,
        n_envs=num_envs,
        seed=seed + num_envs,
        env_kwargs=env_kwargs,
        vec_env_cls=TankVecEnv,
        vec_env_kwargs=vec_env_kwargs,
    )

    render_env = make_vec_env(
        tank_game_environment_v1.env_fn,
        n_envs=1,
        seed=seed + 2 * num_envs,
        env_kwargs=env_kwargs | dict(render_mode="rgb_array"),
        vec_env_cls=TankVecEnv,
        vec_env_kwargs=vec_env_kwargs,
    )

    env_name = render_env.metadata["name"]
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
            "opponent_checkpoint_path": opponent_model_path,
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
    opponent_model_path,
    map_name,
    feature_model_path,
    record_video,
    num_episodes,
    deterministic,
    opponent_deterministic,
):
    """Evaluate an agent."""

    if record_video is not None:
        print(f"Recording video, saving to {record_video}")

    # Configuration variables
    seed = 100
    device = "cpu"
    vec_env_kwargs = dict(
        opponent_model=PPO.load(opponent_model_path, device=device, seed=seed),
        opponent_predict_deterministic=opponent_deterministic,
    )
    env_kwargs = dict(
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
        vec_env_cls=TankVecEnv,
        vec_env_kwargs=vec_env_kwargs,
    )

    eval_env_name = eval_env.metadata["name"]

    # Load model
    print(f"Loading model {model_path}.")
    model = RecurrentPPO.load(model_path, device=device, seed=seed)

    # Log number of wins
    win_log = np.zeros(num_episodes)

    def log_wins_callback(locals_, _):
        done = locals_["done"]
        if done:
            won = "hit" in locals_["infos"][0]
            idx = locals_["episode_counts"][0]
            win_log[idx] = 1 if won else 0

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
                )

                # Reset frame buffer
                frames.clear()

    # Create callback list
    callbacks = [log_wins_callback, log_frames_callback]
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
    print("Win Log: ", win_log)
    print("Win Rate: ", np.count_nonzero(win_log) / win_log.size)


if __name__ == "__main__":

    parser = argparse.ArgumentParser(description="Train or evaluate a model.")

    # Create subparsers for 'train' and 'eval' modes
    subparsers = parser.add_subparsers(dest="mode", required=True)

    # Training mode
    train_parser = subparsers.add_parser("train", help="Run model training.")
    train_parser.add_argument(
        "opponent_model_path", type=str, help="Path to a trained model."
    )
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
    eval_parser.add_argument("model_path", type=str, help="Path to a trained model.")
    eval_parser.add_argument(
        "opponent_model_path", type=str, help="Path to a trained model."
    )
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
        "--deterministic",
        action="store_true",
        help="Perform deterministic inference of the agent or not.",
    )
    eval_parser.add_argument(
        "--opponent-deterministic",
        action="store_true",
        help="Perform deterministic inference of the opponent agent or not.",
    )

    # Parse arguments
    args = parser.parse_args()

    if args.mode == "train":
        print("Training mode selected.")
        train(args.opponent_model_path, args.model_path, args.map, args.feature_model)
    elif args.mode == "eval":
        print("Evaluation mode selected.")
        eval(
            args.model_path,
            args.opponent_model_path,
            args.map,
            args.feature_model,
            args.record_video,
            args.episodes,
            args.deterministic,
            args.opponent_deterministic,
        )
