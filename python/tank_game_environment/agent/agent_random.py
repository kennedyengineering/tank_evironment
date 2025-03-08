# Tank Game (@kennedyengineering)

from .agent_base import ScriptedAgent
from .agent_registry import register_agent


@register_agent
class RandomAgent(ScriptedAgent):
    def policy(self, observation):
        return self.action_space.sample()
