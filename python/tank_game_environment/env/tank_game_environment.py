# Tank Game (@kennedyengineering)
from pettingzoo import ParallelEnv
from pettingzoo.utils import parallel_to_aec, aec_to_parallel, wrappers
from gymnasium.logger import warn
from gymnasium.spaces import Box

from copy import copy

import functools
import numpy as np

from contextlib import redirect_stdout

with redirect_stdout(None):
    import pygame

# TODO: utilize randomness, add option for adding noise to lidar readings
# TODO: replace dummy observation/action spaces
# TODO: dynamically import pygame to support multiprocessing?


def aec_env_fn(**kwargs):
    env = TankGameEnvironment(**kwargs)
    env = parallel_to_aec(env)
    env = wrappers.ClipOutOfBoundsWrapper(env)
    env = wrappers.OrderEnforcingWrapper(env)
    return env


def parallel_env_fn(**kwargs):
    env = aec_env_fn(**kwargs)
    env = aec_to_parallel(env)
    return env


class TankGameEnvironment(ParallelEnv):
    """The metadata holds environment constants."""

    metadata = {
        "name": "tank_game_environment_v0",
        "render_modes": ["human", "rgb_array"],
        "render_fps": 30,
    }

    def __init__(self, render_mode=None):
        """The init method takes in environment arguments.
        These attributes should not be changed after initialization.
        """

        self.render_mode = render_mode
        self.screen = None
        if self.render_mode == "human":
            self.clock = pygame.time.Clock()

        self.timestep = None
        self.possible_agents = ["player_1", "player_2"]

    def reset(self, seed=None, options=None):
        """Reset set the environment to a starting point."""

        if self.screen is None:
            pygame.init()
        if self.render_mode == "human":
            screen_width, screen_height = 800, 600
            self.screen = pygame.display.set_mode((screen_width, screen_height))
            pygame.display.set_caption("Tank Game")

        self.timestep = 0
        self.agents = copy(self.possible_agents)

        observations = {a: np.array([0.1, 0.2], dtype=np.float32) for a in self.agents}

        # Get dummy infos. Necessary for proper parallel_to_aec conversion.
        infos = {a: {} for a in self.agents}

        return observations, infos

    def step(self, actions):
        """Takes in an action for the current agent (specified by agent_selection).
        Update any internal state used by observe() or render().
        """

        # Execute actions

        # Check termination conditions
        terminations = {a: False for a in self.agents}
        rewards = {a: 0 for a in self.agents}

        # Check truncation conditions (overwrites termination conditions)
        truncations = {a: False for a in self.agents}
        self.timestep += 1

        # Get observations
        observations = {a: np.array([0.1, 0.2], dtype=np.float32) for a in self.agents}

        # Get dummy infos (not used)
        infos = {a: {} for a in self.agents}

        if any(terminations.values()) or all(truncations.values()):
            self.agents = []

        if self.render_mode == "human":
            self.render()

        return observations, rewards, terminations, truncations, infos

    def render(self):
        """Renders the environment."""

        if self.render_mode is None:
            warn("You are calling render method without specifying any render mode.")
            return

        # TODO: Render and retrieve frame
        frame = np.ndarray((800, 400, 3), np.uint8)

        if self.render_mode == "human":
            pygame.display.update()
            self.clock.tick(self.metadata["render_fps"])

        return frame if self.render_mode == "rgb_array" else None

    # Observation space should be defined here.
    # If your spaces change over time, remove this line (disable caching).
    @functools.lru_cache(maxsize=None)
    def observation_space(self, agent):
        return Box(0.0, 1.0, shape=(2,), dtype=np.float32)

    # Action space should be defined here.
    # If your spaces change over time, remove this line (disable caching).
    @functools.lru_cache(maxsize=None)
    def action_space(self, agent):
        return Box(0.0, 1.0, shape=(2,), dtype=np.float32)

    def close(self):
        """Uninitialize components."""

        if self.screen is not None:
            pygame.quit()
            self.screen = None
