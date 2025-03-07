# Tank Game (@kennedyengineering)

from .. import tank_game_environment_v0

from ..wrapper.wrapper_agent import AgentWrapper
from ..agent.agent_random import RandomAgent

from gymnasium.utils.env_checker import check_env

import pytest


class TestAgentWrapper:
    def test_gymnasium_check_env(self):

        env = tank_game_environment_v0.parallel_env_fn()

        assert len(env.possible_agents) == 2
        learning_agent = env.possible_agents[0]
        scripted_agent = env.possible_agents[1]

        scripted_policy = RandomAgent(
            env.observation_space(scripted_agent), env.action_space(scripted_agent)
        )

        env = AgentWrapper(env, learning_agent, scripted_agent, scripted_policy)
        # FIXME: test render modes

        check_env(env)
