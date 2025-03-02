# Tank Game (@kennedyengineering)

from tank_game_environment.env.tank_game_environment import parallel_env_fn
from tank_game_environment.wrapper.wrapper_agent_v2 import AgentWrapper


def env_fn(**kwargs):
    """Construct Track and Destroy Environment"""

    env = parallel_env_fn(**kwargs)

    learning_agent = env.possible_agents[0]
    learned_agent = env.possible_agents[1]

    wrapped_env = AgentWrapper(env, learning_agent, learned_agent)
    wrapped_env.metadata = env.metadata
    wrapped_env.metadata["name"] = "tank_game_environment_v2"
    wrapped_env.render_mode = env.render_mode

    return wrapped_env


__all__ = ["env_fn"]
