# Tank Game (@kennedyengineering)
import python_bindings as tank_game

from .tank_game_util import TankData

from pettingzoo import ParallelEnv
from pettingzoo.utils import parallel_to_aec, aec_to_parallel, wrappers
from gymnasium.logger import warn
from gymnasium.spaces import Box, Dict, Discrete

from copy import copy

import functools
import numpy as np

from contextlib import redirect_stdout

with redirect_stdout(None):
    import pygame

# TODO: utilize randomness, add option for adding noise to lidar readings
# TODO: dynamically import pygame to support multiprocessing?


def aec_env_fn(**kwargs):
    """Constructs and returns wrapped AEC environment."""
    env = TankGameEnvironment(**kwargs)
    env = parallel_to_aec(env)
    # env = wrappers.ClipOutOfBoundsWrapper(env)    # FIXME: does not work with dict action space
    env = wrappers.OrderEnforcingWrapper(env)
    return env


def parallel_env_fn(**kwargs):
    """Constructs and returns wrapped Parallel environment."""
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
        self.agent_data = {"tank_1": TankData(), "tank_2": TankData()}
        self.possible_agents = list(self.agent_data.keys())

        self.engine_config = tank_game.Config()

        self.agent_data["tank_1"].config.positionX = self.engine_config.arenaWidth / 3.0
        self.agent_data["tank_1"].config.positionY = (
            self.engine_config.arenaHeight / 2.0
        )
        self.agent_data["tank_1"].config.angle = 0.0

        self.agent_data["tank_2"].config.positionX = (
            2.0 * self.engine_config.arenaWidth / 3.0
        )
        self.agent_data["tank_2"].config.positionY = (
            self.engine_config.arenaHeight / 2.0
        )
        self.agent_data["tank_2"].config.angle = np.pi

    def reset(self, seed=None, options=None):
        """Reset the environment to a starting point."""

        self.engine = tank_game.Engine(self.engine_config)

        for a in self.possible_agents:
            self.agent_data[a].id = self.engine.addTank(self.agent_data[a].config)

        if self.screen is None:
            pygame.init()
        if self.render_mode == "human":
            screen_width, screen_height = self.engine.getImageDimensions()
            self.screen = pygame.display.set_mode((screen_width, screen_height))
            pygame.display.set_caption("Tank Game Environment")

        self.timestep = 0

        self.agents = copy(self.possible_agents)

        observations = {a: self.get_observation(a) for a in self.agents}

        # Get dummy infos. Necessary for proper parallel_to_aec conversion.
        infos = {a: {} for a in self.agents}

        return observations, infos

    def step(self, actions):
        """Takes in actions for the agents."""

        # Execute actions
        for a in actions:
            if a not in self.agents:
                warn(f"You are trying to assign actions to an invalid agent {a}.")
                continue

            action = actions[a]

            self.engine.moveLeftTankTread(
                self.agent_data[a].id,
                action["left_tread_speed"][0] * self.agent_data[a].config.treadMaxSpeed,
            )
            self.engine.moveRightTankTread(
                self.agent_data[a].id,
                action["right_tread_speed"][0]
                * self.agent_data[a].config.treadMaxSpeed,
            )

            # TODO: implement action masking / just prevent from firing
            if action["projectile_fire"]:
                self.engine.fireTankGun(self.agent_data[a].id)

        # Step the engine
        self.engine.step()

        # Get observations
        observations = {a: self.get_observation(a) for a in self.agents}

        # Check termination conditions
        terminations = {a: False for a in self.agents}
        rewards = {a: 0 for a in self.agents}

        # Check truncation conditions (overwrites termination conditions)
        truncations = {a: False for a in self.agents}
        self.timestep += 1

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

        # Render frame
        self.engine.clearImage()

        for a in self.agents:
            self.engine.renderTank(self.agent_data[a].id)
            self.engine.renderTankLidar(self.agent_data[a].id)

        self.engine.renderProjectiles()

        # Retrieve frame
        frame = self.engine.getImageBuffer()

        if self.render_mode == "human":
            surface = pygame.surfarray.make_surface(np.swapaxes(frame, 0, 1))
            self.screen.blit(surface, (0, 0))

            pygame.display.update()
            self.clock.tick(self.metadata["render_fps"])

        return frame if self.render_mode == "rgb_array" else None

    def get_observation(self, agent):
        """Get observation for agent."""

        lidar_scan = self.engine.scanTankLidar(self.agent_data[agent].id)
        lidar_range = self.agent_data[agent].config.lidarRange

        lidar_scan = np.clip(
            lidar_scan, 0.0, lidar_range
        )  # Due to precision errors in simulator, returned distance may be slightly larger than lidar_range
        lidar_scan /= lidar_range  # Normalize between 0.0 and 1.0

        return lidar_scan

    @functools.lru_cache(maxsize=None)
    def observation_space(self, agent):
        """Return agent's observation space."""
        lidar_points = self.agent_data[agent].config.lidarPoints

        return Box(0.0, 1.0, shape=(lidar_points,), dtype=np.float32)

    @functools.lru_cache(maxsize=None)
    def action_space(self, agent):
        """Return agent's action space."""
        return Dict(
            {
                # Tank body
                "left_tread_speed": Box(-1.0, 1.0, shape=(1,), dtype=np.float32),
                "right_tread_speed": Box(-1.0, 1.0, shape=(1,), dtype=np.float32),
                # Projectile
                "projectile_fire": Discrete(2),
            }
        )

    def close(self):
        """Uninitialize components."""

        if self.screen is not None:
            pygame.quit()
            self.screen = None
