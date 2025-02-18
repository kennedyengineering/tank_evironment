# Tank Game (@kennedyengineering)

from tank_game_environment import tank_game_environment_v1

from tank_game_agent.callback.callback_video_recorder import VideoRecorderCallback

import time
import os

import supersuit as ss
from supersuit import frame_stack_v1

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

# TODO: handle determinism / seeds. does there need to be a different seed for train_env and eval_env? (set eval_env to seed, then train_env to seed + 1)


def train():
    """Train an agent."""

    # Configuration variables
    num_envs = 4
    num_eval_episodes = 10
    steps = 1_000_000
    device = "cpu"
    log_dir = "logs/"
    save_dir = "weights/"
    save_freq = 100_000
    eval_freq = 10_000
    batch_size = 256
    verbose = 3

    # Create environments
    # TODO: improve performance with subprocesses
    env = make_vec_env(tank_game_environment_v1.env_fn, n_envs=num_envs)

    eval_env = tank_game_environment_v1.env_fn(render_mode="rgb_array")
    eval_env = Monitor(eval_env)

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

    # Run evaluation (optional)
    eval(os.path.join(save_dir, "best_model"))


def eval(model_path):
    """Evaluate an agent."""

    deterministic = True
    num_episodes = 1

    device = "cpu"

    eval_env = tank_game_environment_v1.env_fn(render_mode="human")
    eval_env = Monitor(eval_env)

    eval_env_name = eval_env.metadata["name"]

    model = PPO.load(model_path, device=device)

    print(f"Starting evaluation on {eval_env_name} (num_episodes={num_episodes})")
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
    train()
