# Tank Game (@kennedyengineering)

from ..agent.agent_base import ScriptedAgent

from pettingzoo import ParallelEnv
from gymnasium import Env


class AgentWrapper(Env):
    """
    A Gymnasium wrapper that turns a two-agent PettingZoo Parallel environment into a single-agent
    environment by replacing one agent with a scripted policy.

    Args:
        pet_env: The PettingZoo multi-agent environment (Parallel-style).
        learning_agent: The agent that will be controlled by your RL algorithm.
        scripted_agent: The agent that will be controlled by a scripted policy.
        scripted_policy: A function that takes an observation (for the scripted agent) and returns an action.
    """

    def __init__(
        self,
        parallel_env: ParallelEnv,
        learning_agent: str,
        scripted_agent: str,
        scripted_policy: ScriptedAgent,
    ):
        super().__init__()

        if not isinstance(parallel_env, ParallelEnv):
            raise TypeError(
                f"Expected parallel_env to be pettingzoo Parallel, got {type(parallel_env)}"
            )
        self._env = parallel_env

        if not isinstance(learning_agent, str):
            raise TypeError(
                f"Expected learning_agent to be str, got {type(parallel_env)}"
            )
        if learning_agent not in parallel_env.possible_agents:
            raise ValueError(
                f"Value of learning_agent is not a valid agent id, passed {learning_agent}"
            )
        self._learning_agent = learning_agent

        if not isinstance(scripted_agent, str):
            raise TypeError(
                f"Expected scripted_agent to be str, got {type(parallel_env)}"
            )
        if scripted_agent not in parallel_env.possible_agents:
            raise ValueError(
                f"Value of scripted_agent is not a valid agent id, passed {scripted_agent}"
            )
        self._scripted_agent = scripted_agent

        if not isinstance(scripted_policy, ScriptedAgent):
            raise TypeError(
                f"Expected scripted_policy to be tank_game_environment ScriptedAgent, got {type(scripted_policy)}"
            )
        self._scripted_policy = scripted_policy

        self._scripted_observation = None

        self.observation_space = self._env.observation_space(learning_agent)
        self.action_space = self._env.action_space(learning_agent)

    def reset(self, seed=None, options=None):
        """
        Reset the underlying PettingZoo environment.
        """
        super().reset(seed=seed)
        self._scripted_policy.action_space.seed(seed=seed)

        observations, infos = self._env.reset(seed=seed, options=options)

        self._scripted_observation = observations[self._scripted_agent]

        return observations[self._learning_agent], infos[self._learning_agent]

    def step(self, action):
        """
        Execute one step in the environment.
        """

        actions = {
            self._learning_agent: action,
            self._scripted_agent: self._scripted_policy.policy(
                self._scripted_observation
            ),
        }

        observations, rewards, terminations, truncations, infos = self._env.step(
            actions
        )

        self._scripted_observation = observations[self._scripted_agent]

        return (
            observations[self._learning_agent],
            rewards[self._learning_agent],
            terminations[self._learning_agent],
            truncations[self._learning_agent],
            infos[self._learning_agent],
        )

    def render(self):
        """
        Render the environment.
        """

        return self._env.render()

    def close(self):
        """
        Uninitialize the environment.
        """

        return self._env.close()
