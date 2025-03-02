# Tank Game (@kennedyengineering)

from pettingzoo import ParallelEnv
from gymnasium import Env


class AgentWrapper(Env):
    """
    A Gymnasium wrapper that turns a two-agent PettingZoo Parallel environment into a single-agent
    environment by replacing one agent with a learned policy.

    Args:
        pet_env: The PettingZoo multi-agent environment (Parallel-style).
        learning_agent: The agent that will be controlled by your RL algorithm.
        learned_agent: The agent that will be controlled by a learned policy.
        learned_policy: A function that takes an observation (for the learned agent) and returns an action.
    """

    def __init__(
        self,
        parallel_env: ParallelEnv,
        learning_agent: str,
        learned_agent: str,
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

        if not isinstance(learned_agent, str):
            raise TypeError(
                f"Expected learned_agent to be str, got {type(parallel_env)}"
            )
        if learned_agent not in parallel_env.possible_agents:
            raise ValueError(
                f"Value of learned_agent is not a valid agent id, passed {learned_agent}"
            )
        self._learned_agent = learned_agent

        self.observation_space = self._env.observation_space(learning_agent)
        self.action_space = self._env.action_space(learning_agent)

    def reset(self, seed=None, options=None):
        """
        Reset the underlying PettingZoo environment.
        """

        super().reset(seed=seed)

        observations, infos = self._env.reset(seed=seed, options=options)

        return (
            observations[self._learning_agent],
            observations[self._learned_agent],
        ), infos[self._learning_agent]

    def step(self, actions):
        """
        Execute one step in the environment.
        """

        actions = {
            self._learning_agent: actions[0],
            self._learned_agent: actions[1],
        }

        observations, rewards, terminations, truncations, infos = self._env.step(
            actions
        )

        self._learned_observation = observations[self._learned_agent]

        return (
            (
                observations[self._learning_agent],
                observations[self._learned_agent],
            ),
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
