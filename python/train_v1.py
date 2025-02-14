# Tank Game (@kennedyengineering)

from tank_game_environment import tank_game_environment_v1

import time
import os

import supersuit as ss
from supersuit import frame_stack_v1

from stable_baselines3 import PPO
from stable_baselines3.ppo import MlpPolicy
from stable_baselines3.common.evaluation import evaluate_policy
from stable_baselines3.common.monitor import Monitor
from stable_baselines3.common.env_util import make_vec_env

# TODO: log videos to tensorboard, and other useful things https://stable-baselines3.readthedocs.io/en/master/guide/tensorboard.html
# TODO: add checkpointing (every n steps, best, and final)


def train():
    """Train an agent."""

    num_envs = 4
    steps = 1_000_000
    seed = 0

    device = "cpu"
    log_dir = "logs/"
    save_dir = "weights/"
    batch_size = 256
    verbose = 3

    env = make_vec_env(tank_game_environment_v1.env_fn, n_envs=num_envs, seed=seed)

    env_name = env.metadata["name"]
    run_name = f"{env_name}_{time.strftime('%Y%m%d-%H%M%S')}"

    model = PPO(
        MlpPolicy,
        env,
        verbose=verbose,
        batch_size=batch_size,
        device=device,
        tensorboard_log=log_dir,
    )

    print(f"Starting training on {env_name}. ({run_name})")
    model.learn(total_timesteps=steps, tb_log_name=run_name)
    print(f"Finished training on {env_name}. ({run_name})")

    model.save(os.path.join(save_dir, run_name))
    print("Model has been saved.")

    env.close()


def eval():
    """Evaluate an agent."""

    deterministic = True
    seed = 0
    num_episodes = 1

    device = "cpu"
    save_dir = "weights/"

    run_name = "tank_game_environment_v1_20250214-231032"

    eval_env = tank_game_environment_v1.env_fn(render_mode="human")
    eval_env = Monitor(eval_env)
    eval_env.reset(seed=seed)

    eval_env_name = eval_env.metadata["name"]

    model = PPO.load(os.path.join(save_dir, run_name), device=device)

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
    # train()
    eval()
