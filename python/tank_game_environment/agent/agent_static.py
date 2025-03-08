# Tank Game (@kennedyengineering)

from .agent_base import ScriptedAgent
from .agent_registry import register_agent

from gymnasium.spaces import Space


@register_agent
class StaticAgent(ScriptedAgent):
    def __init__(self, observation_space: Space, action_space: Space, action=None):
        super().__init__(observation_space, action_space)

        if action is None:
            raise ValueError("An action must be provided for StaticAgent.")

        if not action_space.contains(action):
            raise ValueError(
                f"Invalid action: {action} is not within the action space."
            )

        self.action = action

    def policy(self, observation):
        return self.action
