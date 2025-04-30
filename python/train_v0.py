# Tank Game (@kennedyengineering)

from tank_game_environment import tank_game_environment_v0

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

from stable_baselines3 import PPO
from stable_baselines3.ppo import MlpPolicy
from stable_baselines3.common.callbacks import (
    CallbackList,
    CheckpointCallback,
    EvalCallback,
)
from stable_baselines3.common.vec_env import VecMonitor

import supersuit as ss

from sb3_contrib import RecurrentPPO
from sb3_contrib.ppo_recurrent.policies import MlpLstmPolicy


def train(checkpoint_path, map_name, feature_model_path):
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
    env_kwargs = dict(map_id=map_name)

    feature_model = PPO.load(
        feature_model_path,
        device=device,
        seed=seed,
    )
    feature_model.policy.features_extractor.eval()

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
    # FIXME: concat_vec_envs copies env n_envs times, so the seed is identical for all of envs in vec_env.
    # FIXME: maybe create/modify vec_env to replace supersuit code; have one agent control both players.
    def env_fn(n_envs=1, seed=0, **env_kwargs):
        env = tank_game_environment_v0.parallel_env_fn(**env_kwargs)
        env.reset(
            seed=seed
        )  # FIXME: In "human" mode, calling reset() here initializes PyGame and breaks things later.

        env = FeatureExtractorWrapper(
            env, feature_extractor=feature_model.policy.features_extractor
        )

        env = ss.pettingzoo_env_to_vec_env_v1(env)
        env = ss.concat_vec_envs_v1(env, n_envs, base_class="stable_baselines3")
        env.seed = (
            lambda _: None
        )  # FIXME: Compatibility hack, get PettingZoo and Stable-Baselines3 to cooperate.

        env = VecMonitor(env)

        return env

    env = env_fn(n_envs=num_envs, seed=seed, **env_kwargs)
    eval_env = env_fn(n_envs=num_envs, seed=seed + num_envs, **env_kwargs)
    render_env = env_fn(
        n_envs=1, seed=seed + 2 * num_envs, render_mode="rgb_array", **env_kwargs
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

    # Parse arguments
    args = parser.parse_args()

    if args.mode == "train":
        print("Training mode selected.")
        train(args.model_path, args.map, args.feature_model)
