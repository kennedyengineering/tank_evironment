# Tank Game (@kennedyengineering)

from ..agent.agent_base import ScriptedAgent
from ..agent.agent_static import StaticAgent

from gymnasium.spaces import Box

import numpy as np
import pytest


class TestScriptedAgent:
    @pytest.mark.dependency()
    def test_initialization(self):
        with pytest.raises(TypeError):
            base = ScriptedAgent()

        with pytest.raises(TypeError):
            base = ScriptedAgent([], [])

        base = ScriptedAgent(Box(0, 1), Box(0, 1))

    @pytest.mark.dependency(depends=["TestScriptedAgent::test_initialization"])
    def test_not_implemented(self):
        base = ScriptedAgent(Box(0, 1), Box(0, 1))

        with pytest.raises(NotImplementedError):
            base.policy([])


class TestStaticAgent:
    @pytest.mark.dependency(depends=["TestScriptedAgent::test_initialization"])
    def test_initialization(self):
        with pytest.raises(ValueError):
            static = StaticAgent(Box(0, 1), Box(0, 1))

        with pytest.raises(ValueError):
            static = StaticAgent(Box(0, 1), Box(0, 1), [2])

        static = StaticAgent(Box(0, 1), Box(0, 1), [0.5])

    @pytest.mark.dependency(depends=["TestStaticAgent::test_initialization"])
    def test_inheritance(self):
        static = StaticAgent(Box(0, 1), Box(0, 1), [0.5])
        assert isinstance(static, ScriptedAgent)

    @pytest.mark.dependency(depends=["TestStaticAgent::test_initialization"])
    def test_static_policy(self):
        static = StaticAgent(Box(0, 1), Box(0, 1), [0.5])
        assert static.policy([]) == [0.5]

        static = StaticAgent(Box(0, 1), Box(0, 1, shape=(3,)), [0.5, 0.5, 0.5])
        assert static.policy([]) == [0.5, 0.5, 0.5]
