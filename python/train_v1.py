# Tank Game (@kennedyengineering)

from tank_game_environment import tank_game_environment_v1

from tank_game_agent.callback.callback_video_recorder import VideoRecorderCallback

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
from stable_baselines3.common.vec_env import SubprocVecEnv


def train():
    """Train an agent."""

    # Configuration variables
    num_envs = 4
    num_eval_episodes = 10
    steps = 1_000_000
    seed = 0
    device = "cpu"
    log_dir = "logs/"
    save_dir = "weights/"
    save_freq = 100_000
    eval_freq = 10_000
    batch_size = 256
    verbose = 3

    # Create environments
    env = make_vec_env(
        tank_game_environment_v1.env_fn,
        n_envs=num_envs,
        seed=seed,
        start_index=1,
        vec_env_cls=SubprocVecEnv,
    )

    eval_env = tank_game_environment_v1.env_fn(render_mode="rgb_array")
    eval_env = Monitor(eval_env)
    eval_env.reset(seed=seed)

    env_name = eval_env.metadata["name"]
    run_name = f"{env_name}_{time.strftime('%Y%m%d-%H%M%S')}"
    save_dir = os.path.join(save_dir, run_name)

    # Create model
    model = PPO(
        MlpPolicy,
        env,
        verbose=verbose,
        batch_size=batch_size,
        device=device,
        tensorboard_log=log_dir,
        seed=seed,
    )

    # Setup callbacks
    checkpoint_callback = CheckpointCallback(
        save_freq=max(save_freq // num_envs, 1),
        save_path=save_dir,
        name_prefix=run_name,
        verbose=verbose,
    )
    video_callback = VideoRecorderCallback(eval_env=eval_env, render_freq=1)
    eval_callback = EvalCallback(
        eval_env=eval_env,
        callback_on_new_best=video_callback,
        n_eval_episodes=num_eval_episodes,
        eval_freq=max(eval_freq // num_envs, 1),
        best_model_save_path=save_dir,
        verbose=verbose,
    )
    callbacks = CallbackList([checkpoint_callback, eval_callback])

    # Train model
    print(f"Starting training on {env_name}. ({run_name})")
    model.learn(total_timesteps=steps, tb_log_name=run_name, callback=callbacks)
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
    num_episodes = 1
    device = "cpu"

    # Create environment
    eval_env = tank_game_environment_v1.env_fn(render_mode="human")
    eval_env = Monitor(eval_env)

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

    # Evaluation mode
    eval_parser = subparsers.add_parser("eval", help="Run model evaluation.")
    eval_parser.add_argument("model_path", type=str, help="Path to the trained model.")

    # Parse arguments
    args = parser.parse_args()

    if args.mode == "train":
        print("Training mode selected.")
        train()
    elif args.mode == "eval":
        print("Evaluation mode selected.")
        eval(args.model_path)
