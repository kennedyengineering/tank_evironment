# Tank Game (@kennedyengineering)
from pettingzoo import ParallelEnv
from gymnasium.spaces import Box

import functools
import numpy as np


class TankGameEnvironment(ParallelEnv):
    """The metadata holds environment constants."""

    metadata = {
        "name": "tank_game_environment_v0",
    }

    def __init__(self):
        """The init method takes in environment arguments.
        These attributes should not be changed after initialization.
        """

        pass

    def reset(self, seed=None, options=None):
        """Reset set the environment to a starting point."""

        observations = None

        # Get dummy infos. Necessary for proper parallel_to_aec conversion.
        infos = {a: {} for a in self.agents}

        return observations, infos

    def step(self, actions):
        """Takes in an action for the current agent (specified by agent_selection).
        Update any internal state used by observe() or render().
        """

        observations = None
        rewards = None
        terminations = None
        truncations = None

        # Get dummy infos (not used)
        infos = {a: {} for a in self.agents}

        return observations, rewards, terminations, truncations, infos

    def render(self):
        """Renders the environment."""
        # TODO: import pygame, create window, display rendered frame

        pass

    # Observation space should be defined here.
    # If your spaces change over time, remove this line (disable caching).
    @functools.lru_cache(maxsize=None)
    def observation_space(self, agent):
        return Box(0, 1, shape=(2,))

    # Action space should be defined here.
    # If your spaces change over time, remove this line (disable caching).
    @functools.lru_cache(maxsize=None)
    def action_space(self, agent):
        return Box(0, 1, shape=(2,))
