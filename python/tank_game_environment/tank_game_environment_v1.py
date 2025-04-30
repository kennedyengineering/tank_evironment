# Tank Game (@kennedyengineering)

from tank_game_environment.env.tank_game_environment import parallel_env_fn
from tank_game_environment.wrapper.wrapper_agent import AgentWrapper
from tank_game_environment.agent.agent_registry import create_agent

from typing import Optional, List, Type, Dict, Any
from pettingzoo.utils.wrappers import BaseParallelWrapper


def env_fn(
    scripted_policy_name: Optional[str] = None,
    scripted_policy_kwargs: Optional[dict] = None,
    wrappers: Optional[List[Type[BaseParallelWrapper]]] = None,
    wrappers_kwargs: Optional[Dict[Type[BaseParallelWrapper], Dict[str, Any]]] = None,
    **kwargs
):
    """
    Construct Track and Destroy Environment.

    Args:
        scripted_policy_name: Optional name of the scripted policy to be used.
        scripted_policy_kwargs: Optional dictionary of keyword arguments for the scripted policy.
        wrappers: Optional wrappers to apply to base environment.
        wrappers_kwargs: Optional wrapper initialization arguments.
        **kwargs: Additional keyword arguments to pass to parallel_env_fn.

    Returns:
        A Gymnasium environment.
    """

    env = parallel_env_fn(**kwargs)

    # Apply wrappers
    if wrappers:
        for wrapper_cls in wrappers:
            init_kwargs = (
                wrappers_kwargs.get(wrapper_cls, {}) if wrappers_kwargs else {}
            )
            env = wrapper_cls(env, **init_kwargs)

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
