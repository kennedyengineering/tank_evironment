# Tank Game (@kennedyengineering)

from tank_game_environment import tank_game_environment_v1

from tank_game_agent.callback.callback_video_recorder import VideoRecorderCallback
from tank_game_agent.callback.callback_hparam_recorder import HParamRecorderCallback

from tank_game_agent.schedule.schedule_linear import linear_schedule

from tank_game_agent.feature_extactor.feature_extractor_lidar import LidarCNN

import time
import os
import argparse

from stable_baselines3 import PPO
from stable_baselines3.ppo import MlpPolicy
from stable_baselines3.common.evaluation import evaluate_policy
from stable_baselines3.common.monitor import Monitor
from stable_baselines3.common.env_util import make_vec_env
from stable_baselines3.common.callbacks import (
    CallbackList,
    CheckpointCallback,
    EvalCallback,
)
from stable_baselines3.common.vec_env import SubprocVecEnv, DummyVecEnv


def train(checkpoint_path=None):
    """Train an agent."""

    # Configuration variables
    num_envs = 12
    num_eval_episodes = 10
    steps = 6_000_000
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
        features_extractor_kwargs=dict(features_dim=128),
    )
    env_kwargs = dict(
        scripted_policy_name="StaticAgent",
        scripted_policy_kwargs=dict(action=[0.0, 0.0, 0.0]),
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
        vec_env_cls=SubprocVecEnv,
    )

    eval_env = make_vec_env(
        tank_game_environment_v1.env_fn,
        n_envs=num_envs,
        env_kwargs=env_kwargs,
        seed=seed,
        start_index=seed
        + num_envs,  # FIXME: iterate seed instead of start_index (leave at default = 0), env and eval env have identical seeding. start_index only changes seeding for environment action space. will break from the repeatability of previous experiments though.
        vec_env_cls=SubprocVecEnv,
    )

    render_env = make_vec_env(
        tank_game_environment_v1.env_fn,
        n_envs=1,
        env_kwargs=env_kwargs | dict(render_mode="rgb_array"),
        seed=seed,
        start_index=seed + num_envs + 1,
        vec_env_cls=DummyVecEnv,
    )

    env_name = render_env.metadata["name"]
    run_name = f"{env_name}_{time.strftime('%Y%m%d-%H%M%S')}"
    save_dir = os.path.join(save_dir, run_name)

    # Handle scheduling learning rate and clip range
    ppo_config_copy = ppo_config.copy()
    if schedule_learning_rate:
        ppo_config["learning_rate"] = linear_schedule(ppo_config["learning_rate"])
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
            seed=seed,  # FIXME: Also, investigate how this seed affects the seeding of env
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


def eval(model_path):
    """Evaluate an agent."""

    # Configuration variables
    deterministic = True
    num_episodes = 20
    device = "cuda"
    env_kwargs = dict(
        scripted_policy_name="StaticAgent",
        scripted_policy_kwargs=dict(action=[0.0, 0.0, 0.0]),
    )

    # Create environment
    eval_env = make_vec_env(
        tank_game_environment_v1.env_fn,
        n_envs=1,
        env_kwargs=env_kwargs | dict(render_mode="human"),
        vec_env_cls=DummyVecEnv,
    )

    eval_env_name = eval_env.metadata["name"]

    # Load model
    print(f"Loading model {model_path}.")
    model = PPO.load(model_path, device=device)

    # Run evaluation
    print(f"Starting evaluation on {eval_env_name}. (num_episodes={num_episodes})")
    rewards = evaluate_policy(
        model,
        eval_env,
        n_eval_episodes=num_episodes,
        deterministic=deterministic,
        render=False,
        return_episode_rewards=True,
    )
    print("Rewards: ", rewards)


if __name__ == "__main__":

    parser = argparse.ArgumentParser(description="Train or evaluate a model.")

    # Create subparsers for 'train' and 'eval' modes
    subparsers = parser.add_subparsers(dest="mode", required=True)

    # Training mode
    train_parser = subparsers.add_parser("train", help="Run model training.")
    train_parser.add_argument(
        "model_path", type=str, nargs="?", help="Path to the model checkpoint."
    )

    # Evaluation mode
    eval_parser = subparsers.add_parser("eval", help="Run model evaluation.")
    eval_parser.add_argument("model_path", type=str, help="Path to the trained model.")

    # Parse arguments
    args = parser.parse_args()

    if args.mode == "train":
        print("Training mode selected.")
        train(args.model_path)
    elif args.mode == "eval":
        print("Evaluation mode selected.")
        eval(args.model_path)
