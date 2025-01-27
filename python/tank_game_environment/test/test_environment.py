# Tank Game (@kennedyengineering)

from ..env.tank_game_environment import parallel_env_fn, aec_env_fn
from pettingzoo.test import (
    parallel_api_test,
    parallel_seed_test,
    api_test,
    seed_test,
    render_test,
    performance_benchmark,
)

import pytest


class TestParallelEnvironment:
    def test_parallel_api(self):
        parallel_api_test(parallel_env_fn())

    def test_parallel_seed(self):
        parallel_seed_test(parallel_env_fn)


class TestAECEnvironment:
    def test_api(self):
        api_test(aec_env_fn())

    def test_seed(self):
        seed_test(aec_env_fn)

    def test_render(self):
        render_test(aec_env_fn)

    def test_performance(self):
        performance_benchmark(aec_env_fn())
