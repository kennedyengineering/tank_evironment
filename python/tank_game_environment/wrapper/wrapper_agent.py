# Tank Game (@kennedyengineering)

from ..agent.agent_base import ScriptedAgent

from typing import Union

from pettingzoo import ParallelEnv
from gymnasium import Env


class AgentWrapper(Env):
    """
    A Gymnasium wrapper that turns a two-agent PettingZoo Parallel environment into a single-agent
    environment by replacing one agent with a known policy.

    Args:
        pet_env: The PettingZoo multi-agent environment (Parallel-style).
        learning_agent: The agent that will be controlled by your RL algorithm.
        opponent_agent: The agent that will be controlled by a known policy.
        opponent_policy: A ScriptedAgent to stand in for the opponent agent, or None to enter manual mode for use with an external policy function.
    """

    def __init__(
        self,
        parallel_env: ParallelEnv,
        learning_agent: str,
        opponent_agent: str,
        opponent_policy: Union[None, ScriptedAgent] = None,
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

        if not isinstance(opponent_agent, str):
            raise TypeError(
                f"Expected opponent_agent to be str, got {type(parallel_env)}"
            )
        if opponent_agent not in parallel_env.possible_agents:
            raise ValueError(
                f"Value of opponent_agent is not a valid agent id, passed {opponent_agent}"
            )
        self._opponent_agent = opponent_agent

        if opponent_policy is not None and not isinstance(
            opponent_policy, ScriptedAgent
        ):
            raise TypeError(
                f"Expected opponent_policy to be tank_game_environment ScriptedAgent, got {type(opponent_policy)}"
            )
        self._opponent_policy = opponent_policy

        self._opponent_observation = None
        self._opponent_action = None

        self.metadata = self._env.metadata
        self.render_mode = self._env.render_mode

        self.observation_space = self._env.observation_space(learning_agent)
        self.action_space = self._env.action_space(learning_agent)

    def get_opponent_observation(self):
        """
        (For external policy function)
        Return the latest observation for the opponent agent.
        Valid
        """

        return self._opponent_observation

    def set_opponent_action(self, action):
        """
        (For external policy function)
        Set the next action for the opponent agent.
        """

        self._opponent_action = action

    def reset(self, seed=None, options=None):
        """
        Reset the underlying PettingZoo environment.
        """

        # Handle seeding
        super().reset(seed=seed)

        if self._opponent_policy:
            self._opponent_policy.action_space.seed(seed=seed)

        # Reset the environment
        observations, infos = self._env.reset(seed=seed, options=options)

        # Store opponent observation
        self._opponent_observation = observations[self._opponent_agent]

        # Reset opponent action
        self._opponent_action = None

        return observations[self._learning_agent], infos[self._learning_agent]

    def step(self, action):
        """
        Execute one step in the environment.
        """

        # Handle actions
        if self._opponent_policy:
            self._opponent_action = self._opponent_policy.policy(
                self._opponent_observation
            )

        if self._opponent_action is None:
            raise RuntimeError("No opponent action determined.")

        actions = {
            self._learning_agent: action,
            self._opponent_agent: self._opponent_action,
        }

        # Step the environment
        observations, rewards, terminations, truncations, infos = self._env.step(
            actions
        )

        # Store opponent observation
        self._opponent_observation = observations[self._opponent_agent]

        # Reset opponent action
        self._opponent_action = None

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
