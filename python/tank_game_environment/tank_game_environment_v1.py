# Tank Game (@kennedyengineering)

from tank_game_environment.env.tank_game_environment import parallel_env_fn
from tank_game_environment.wrapper.wrapper_agent import AgentWrapper
from tank_game_environment.agent.agent_registry import create_agent

from supersuit import frame_stack_v2

from typing import Optional


def env_fn(
    scripted_policy_name: Optional[str] = None,
    scripted_policy_kwargs: Optional[dict] = None,
    **kwargs
):
    """
    Construct Track and Destroy Environment.

    Args:
        scripted_policy_name: Optional name of the scripted policy to be used.
        scripted_policy_kwargs: Optional dictionary of keyword arguments for the scripted policy.
        **kwargs: Additional keyword arguments to pass to parallel_env_fn.

    Returns:
        A Gymnasium environment.
    """

    env = parallel_env_fn(**kwargs)
    env = frame_stack_v2(env, stack_size=4, stack_dim=-1)

    # Ensure there are at least two agents.
    if len(env.possible_agents) < 2:
        raise ValueError("Expected at least two agents in the environment.")

    learning_agent, opponent_agent = env.possible_agents[:2]

    scripted_policy = None
    if scripted_policy_name:
        observation_space = env.observation_space(opponent_agent)
        action_space = env.action_space(opponent_agent)

        if scripted_policy_kwargs is not None:
            scripted_policy = create_agent(
                scripted_policy_name,
                observation_space,
                action_space,
                **scripted_policy_kwargs,
            )
        else:
            scripted_policy = create_agent(
                scripted_policy_name, observation_space, action_space
            )

    wrapped_env = AgentWrapper(env, learning_agent, opponent_agent, scripted_policy)
    wrapped_env.metadata["name"] = "tank_game_environment_v1"

    return wrapped_env


__all__ = ["env_fn"]
