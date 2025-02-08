# Tank Game (@kennedyengineering)

from .agent_base import ScriptedAgent


class RandomAgent(ScriptedAgent):
    def policy(self, observation):
        return self.action_space.sample()
