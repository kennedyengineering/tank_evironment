# Tank Game (@kennedyengineering)

import python_bindings as tank_game

import pytest


class TestBinding:
    def test_engine_destroy(self):
        config = tank_game.Config()

        for i in range(100000):
            engine = tank_game.Engine(config)
