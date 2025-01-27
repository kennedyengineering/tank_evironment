# Tank Game (@kennedyengineering)

from ..env.tank_game_environment import TankGameEnvironment
from pettingzoo.test import (
    parallel_api_test,
    parallel_seed_test,
    render_test,
    performance_benchmark,
)

import pytest


class TestEnvironment:
    def test_parallel_api(self):
        env = TankGameEnvironment()
        parallel_api_test(env, num_cycles=1_000_000)

    def test_parallel_seed(self):
        env = TankGameEnvironment()
        parallel_seed_test(env)

    def test_render(self):
        env = TankGameEnvironment()
        render_test(env)

    def test_performance(self):
        env = TankGameEnvironment()
        performance_benchmark(env)
