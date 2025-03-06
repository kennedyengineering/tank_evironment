# Tank Game (@kennedyengineering)

from tank_game_environment.env.tank_game_environment import parallel_env_fn
from tank_game_environment.agent.agent_static import StaticAgent
from tank_game_environment.wrapper.wrapper_agent_v1 import AgentWrapper

from supersuit import frame_stack_v2

import numpy as np


def env_fn(**kwargs):
    """Construct Seek and Destroy Environment"""

    env = parallel_env_fn(**kwargs)
    env = frame_stack_v2(env, stack_size=4, stack_dim=-1)

    learning_agent = env.possible_agents[0]
    scripted_agent = env.possible_agents[1]

    observation_space = env.observation_space(scripted_agent)
    action_space = env.action_space(scripted_agent)

    scripted_policy = StaticAgent(
        observation_space,
        action_space,
        np.array([0.0, 0.0, 0.0], dtype=action_space.dtype),
    )

    wrapped_env = AgentWrapper(env, learning_agent, scripted_agent, scripted_policy)
    wrapped_env.metadata = env.metadata
    wrapped_env.metadata["name"] = "tank_game_environment_v1"

    return wrapped_env


__all__ = ["env_fn"]
