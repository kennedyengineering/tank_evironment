# Tank Game (@kennedyengineering)

from tank_game_environment import tank_game_environment_v0

import time
import os
import glob

import supersuit as ss
from supersuit import frame_stack_v1

from stable_baselines3 import PPO
from stable_baselines3.ppo import MlpPolicy

# TODO: log videos to tensorboard, and other useful things https://stable-baselines3.readthedocs.io/en/master/guide/tensorboard.html
# TODO: add checkpointing (every n steps, best, and final)
# TODO: ^^ use a more compact filesystem (ex: runs/run_1/logs + /weights)


def train_env_supersuit(steps: int = 100_000, seed: int | None = 0, **env_kwargs):
    """Train an agent using the Parallel API."""

    env = tank_game_environment_v0.parallel_env_fn(**env_kwargs)

    # TODO: Check environment is reproducable. Here the seed is set, only happens once.
    # FIXME: In "human" mode, calling reset() here initializes PyGame and breaks things later.
    # env.reset(seed)

    print(f"Starting training on {str(env.metadata['name'])}.")

    env = frame_stack_v1(env, 4)

    env = ss.pettingzoo_env_to_vec_env_v1(env)
    env = ss.concat_vec_envs_v1(env, 8, num_cpus=2, base_class="stable_baselines3")

    model = PPO(
        MlpPolicy, env, verbose=3, batch_size=256, device="cpu", tensorboard_log="logs/"
    )

    run_name = f"{env.unwrapped.metadata.get('name')}_{time.strftime('%Y%m%d-%H%M%S')}"

    model.learn(total_timesteps=steps, tb_log_name=run_name)

    model.save(f"weights/{run_name}")

    print("Model has been saved.")

    print(f"Finished training on {str(env.unwrapped.metadata['name'])}.")

    env.close()


def eval(num_games: int = 100, render_mode: str | None = None, **env_kwargs):
    """Evaluate an agent using the AEC API."""

    env = tank_game_environment_v0.aec_env_fn(render_mode=render_mode, **env_kwargs)

    env = frame_stack_v1(env, 4)

    print(
        f"\nStarting evaluation on {str(env.metadata['name'])} (num_games={num_games}, render_mode={render_mode})"
    )

    try:
        latest_policy = max(
            glob.glob(f"weights/{env.metadata['name']}*.zip"), key=os.path.getctime
        )
    except ValueError:
        print("Policy not found.")
        exit(0)

    model = PPO.load(latest_policy, device="cpu")

    rewards = {agent: 0 for agent in env.possible_agents}

    for i in range(num_games):
        env.reset(seed=i)

        for agent in env.agent_iter():
            obs, reward, termination, truncation, info = env.last()

            for a in env.agents:
                rewards[a] += env.rewards[a]
            if termination or truncation:
                break
            else:
                act = model.predict(obs, deterministic=True)[0]

            env.step(act)
    env.close()

    avg_reward = sum(rewards.values()) / len(rewards.values())
    print("Rewards: ", rewards)
    print(f"Avg reward: {avg_reward}")
    return avg_reward


if __name__ == "__main__":
    env_kwargs = {}

    # Train a model (takes ~3 minutes on GPU)
    train_env_supersuit(steps=1_000_000, seed=0, **env_kwargs)

    # Evaluate 10 games (average reward should be positive but can vary significantly)
    eval(num_games=10, render_mode=None, **env_kwargs)

    # Watch 2 games
    eval(num_games=2, render_mode="human", **env_kwargs)
