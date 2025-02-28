# Tank Game (@kennedyengineering)

from tank_game_environment.env.tank_game_environment import parallel_env_fn
from tank_game_environment.agent.agent_static import StaticAgent
from tank_game_environment.wrapper.wrapper_agent import AgentWrapper

import numpy as np


def env_fn(**kwargs):
    """Construct Track and Destroy Environment"""

    env = parallel_env_fn(**kwargs)

    learning_agent = env.possible_agents[0]
    scripted_agent = env.possible_agents[1]

    observation_space = env.observation_space(scripted_agent)
    action_space = env.action_space(scripted_agent)

    scripted_policy = StaticAgent(
        observation_space,
        action_space,
        np.array([1.0, 0.75, 0.0], dtype=action_space.dtype),
    )

    wrapped_env = AgentWrapper(env, learning_agent, scripted_agent, scripted_policy)
    wrapped_env.metadata = env.metadata
    wrapped_env.metadata["name"] = "tank_game_environment_v2"

    return wrapped_env


__all__ = ["env_fn"]
