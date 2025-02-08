# Tank Game (@kennedyengineering)

from gymnasium.spaces import Space


class ScriptedAgent:
    def __init__(self, observation_space: Space, action_space: Space):
        if not isinstance(observation_space, Space):
            raise TypeError(
                f"Expected observation_space to be a gymnasium Space, got {type(observation_space)}"
            )

        if not isinstance(action_space, Space):
            raise TypeError(
                f"Expected action_space to be a gymnasium Space, got {type(action_space)}"
            )

        self.observation_space = observation_space
        self.action_space = action_space

    def policy(self, observation):
        """A placeholder for the agent's policy that maps observations to actions."""

        raise NotImplementedError("Please implement in subclass.")
