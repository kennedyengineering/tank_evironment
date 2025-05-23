# Tank Game (@kennedyengineering)
import python_bindings as tank_game

from .tank_game_util import TankData

from ..map.map_registry import registry as map_registry

from pettingzoo import ParallelEnv
from pettingzoo.utils import parallel_to_aec, aec_to_parallel, wrappers
from gymnasium.logger import (
    warn,
    error,
)  # FIXME: do something about the error conditions
from gymnasium.spaces import Box
from gymnasium.utils import EzPickle

from copy import copy

import functools
import numpy as np
import pathlib

from contextlib import redirect_stdout


def aec_env_fn(**kwargs):
    """Constructs and returns wrapped AEC environment."""
    env = TankGameEnvironment(**kwargs)
    env = parallel_to_aec(env)
    env = wrappers.ClipOutOfBoundsWrapper(env)
    env = wrappers.OrderEnforcingWrapper(env)
    return env


def parallel_env_fn(**kwargs):
    """Constructs and returns wrapped Parallel environment."""
    env = aec_env_fn(**kwargs)
    env = aec_to_parallel(env)
    return env


class TankGameEnvironment(ParallelEnv, EzPickle):
    """
    The metadata holds environment constants.
    - name : for pretty printing
    - render_modes : valid rendering modes
    - render_fps : for "human" rendering mode
    - max_timesteps : how many steps before truncation (-1 for no truncation)
    - reload_delay : how many steps before tank can fire (0 for no delay)

    The engine_metadata holds engine constants.
    - pixel_density : pixels per meters (for rendering)
    - verbose_output : enable engine stdout messages

    The tank_metadata holds tank constants.
    - tread_max_speed : maximum linear velocity of an agent (meters / second)
    - lidar_range : range of the lidar (meters)
    - lidar_points : number of points in a lidar scan
    - lidar_pixel_radius : radius in pixels (for rendering)
    """

    metadata = {
        "name": "tank_game_environment_v0",
        "render_modes": ["human", "rgb_array"],
        "render_fps": 30,
        "max_timesteps": 1000,
        "reload_delay": 20,
    }

    engine_metadata = {
        "pixel_density": 2.0,
        "verbose_output": False,
    }

    tank_metadata = {
        "tread_max_speed": 20.0,
        "lidar_range": 35.0,
        "lidar_points": 360,
        "lidar_pixel_radius": 1.0,
    }

    def __init__(self, render_mode: str = None, map_id: str = "Random"):
        """The init method takes in environment arguments.
        These attributes should not be changed after initialization.
        """

        EzPickle.__init__(self, render_mode=render_mode, map_id=map_id)

        # load map class
        if map_id not in map_registry:
            error("Invalid map id.")
        self.map_cls = map_registry[map_id]

        # define engine variables
        self.engine = None
        self.agent_data = None
        self.obstacle_ids = None

        # define rendering variables
        self.render_mode = render_mode
        self.screen = None
        self.clock = None

        # define environment variables
        self.possible_agents = [
            f"tank_{i}" for i in range(self.map_cls.get_num_tanks())
        ]
        self.agents = None
        self.timestep = None

    def __construct_map(self):
        """Configures the arena according to the specification defined in the map."""

        # initialize map
        map_inst = self.map_cls()

        # construct engine
        engine_config = tank_game.Config()
        engine_config.arenaWidth = map_inst.arena_map_data.width
        engine_config.arenaHeight = map_inst.arena_map_data.height
        engine_config.pixelDensity = self.engine_metadata["pixel_density"]
        engine_config.verboseOutput = self.engine_metadata["verbose_output"]

        self.engine = tank_game.Engine(engine_config)

        # construct agents
        self.agent_data = dict()
        for agent_data_key, tank_map_data in zip(
            self.possible_agents, map_inst.tank_map_data
        ):
            tank_config = tank_game.TankConfig()
            tank_config.positionX = tank_map_data.position_x
            tank_config.positionY = tank_map_data.position_y
            tank_config.angle = tank_map_data.angle
            tank_config.treadMaxSpeed = self.tank_metadata["tread_max_speed"]
            tank_config.lidarPoints = self.tank_metadata["lidar_points"]
            tank_config.lidarRange = self.tank_metadata["lidar_range"]
            tank_config.lidarRadius = self.tank_metadata["lidar_pixel_radius"]

            tank_id = self.engine.addTank(tank_config)

            self.agent_data[agent_data_key] = TankData(tank_id, 0)

        # construct obstacles
        self.obstacle_ids = list()
        for obstacle_map_data in map_inst.obstacle_map_data:
            obstacle_config = tank_game.ObstacleConfig()
            obstacle_config.positionX = obstacle_map_data.position_x
            obstacle_config.positionY = obstacle_map_data.position_y
            obstacle_config.radius = obstacle_map_data.radius

            obstacle_id = self.engine.addObstacle(obstacle_config)

            self.obstacle_ids.append(obstacle_id)

    def reset(self, seed=None, options=None):
        """Reset the environment to a starting point."""

        # set seed
        if seed is not None:
            np.random.seed(seed=seed)

        # reset engine variables
        self.__construct_map()

        # reset environment variables
        self.possible_agents = list(self.agent_data.keys())
        self.agents = copy(self.possible_agents)
        self.timestep = 0

        # get initial observations
        observations = {a: self.get_observation(a) for a in self.agents}

        # get dummy infos (necessary for proper parallel_to_aec conversion)
        infos = {a: {} for a in self.agents}

        # render environment (optional)
        if self.render_mode == "human":
            self.render()

        return observations, infos

    def __get_agent_from_id(self, id):
        """Search for matching TankData.id in agent_data."""

        agents = [key for key, value in self.agent_data.items() if value.id == id]

        if len(agents) == 0:
            error("Invalid agent id.")

        if len(agents) > 1:
            error("Multiple matches for agent id.")

        agent = agents[0]

        return agent

    def __shape_agent_reward(self, observation):
        """Shapes agent reward."""

        # TODO: add metadata for simple reward shaping configuration

        reward = 0

        # penalize every timestep
        reward -= 1

        # reward movement
        reward += abs(observation[360]) * 0.5
        reward += observation[361] * 0.5

        # punish rotation
        # reward -= abs(observation[362])*0.2

        return reward

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
                action[0] * self.tank_metadata["tread_max_speed"],
            )
            self.engine.moveRightTankTread(
                self.agent_data[a].id,
                action[1] * self.tank_metadata["tread_max_speed"],
            )

            if action[2] > 0.0 and self.agent_data[a].reload_counter == 0:
                self.engine.fireTankGun(self.agent_data[a].id)

        # Handle reload counter
        for a in self.agents:
            if self.agent_data[a].reload_counter == 0:
                self.agent_data[a].reload_counter = self.metadata["reload_delay"]
            else:
                self.agent_data[a].reload_counter -= 1

        # Step the engine
        projectile_events = self.engine.step()

        # Get observations
        observations = {a: self.get_observation(a) for a in self.agents}

        # Assign rewards
        rewards = {a: self.__shape_agent_reward(observations[a]) for a in self.agents}

        # Check termination conditions
        terminations = {a: False for a in self.agents}

        filtered_events = [
            x for x in projectile_events if x[0] == tank_game.CategoryBits.TANK_BODY
        ]
        unique_events = list(
            set(filtered_events)
        )  # Account for a tank being hit multiple times

        # FIXME: only give termination to the tank that was shot?
        if unique_events:
            terminations = {a: True for a in self.agents}

        # FIXME: if both get shot at same time, give both negative reward instead of tie?
        # FIXME: what if tank hits itself?
        for event in unique_events:
            src_agent_id = event[1]
            src_agent = self.__get_agent_from_id(src_agent_id)
            if src_agent not in self.agents:
                error("Rewarding an invalid agent.")
            rewards[src_agent] += 100

            hit_agent_id = event[2]
            hit_agent = self.__get_agent_from_id(hit_agent_id)
            if hit_agent not in self.agents:
                error("Rewarding an invalid agent.")
            rewards[hit_agent] -= 100

        # Check truncation conditions
        truncations = {a: False for a in self.agents}
        if (
            self.timestep > self.metadata["max_timesteps"]
            and self.metadata["max_timesteps"] != -1
        ):
            truncations = {a: True for a in self.agents}
        self.timestep += 1

        # Get dummy infos (not used)
        infos = {a: {} for a in self.agents}

        if any(terminations.values()) or all(truncations.values()):
            self.agents = []

        if self.render_mode == "human":
            self.render()

        return observations, rewards, terminations, truncations, infos

    def render(self):
        """Render the environment."""
        # TODO: add debug render option. overlays FPS, etc.

        # Check render mode
        if self.render_mode is None:
            warn("You are calling render method without specifying any render mode.")
            return

        # Setup PyGame
        with redirect_stdout(None):
            global pygame
            import pygame

        if self.screen is None and self.render_mode == "human":
            pygame.init()

            screen_width, screen_height = self.engine.getImageDimensions()
            self.screen = pygame.display.set_mode((screen_width, screen_height))
            pygame.display.set_caption("Tank Game Environment")

            icon_path = (
                pathlib.Path(__file__).parent.parent.resolve() / "asset/tank_icon.png"
            )
            pygame.display.set_icon(pygame.image.load(icon_path))

        if self.clock is None and self.render_mode == "human":
            self.clock = pygame.time.Clock()

        # Render frame
        self.engine.clearImage()

        for id in self.obstacle_ids:
            self.engine.renderObstacle(id)

        for a in self.agents:
            id = self.agent_data[a].id
            self.engine.renderTank(id)
            self.engine.renderTankLidar(id)

        self.engine.renderProjectiles()

        # Retrieve frame
        frame = self.engine.getImageBuffer()

        # Display frame
        if self.render_mode == "human":
            surface = pygame.surfarray.make_surface(np.swapaxes(frame, 0, 1))
            self.screen.blit(surface, (0, 0))

            pygame.event.pump()
            pygame.display.update()
            self.clock.tick(self.metadata["render_fps"])

        return frame if self.render_mode == "rgb_array" else None

    def get_observation(self, agent):
        """Get observation for agent."""

        # FIXME: remove clipping to account for brief moments when values are beyond defined ranges?

        id = self.agent_data[agent].id

        # obtain lidar observation
        lidar_scan = self.engine.scanTankLidar(id)
        lidar_range = self.tank_metadata["lidar_range"]

        lidar_scan = np.clip(
            lidar_scan, 0.0, lidar_range
        )  # Due to precision errors in simulator, returned distance may be slightly larger than lidar_range
        lidar_scan /= lidar_range  # Normalize between 0.0 and 1.0

        # obtain velocity observation
        velocity = self.engine.getTankLocalVelocity(id)
        velocity_range = self.tank_metadata["tread_max_speed"]

        velocity = np.clip(
            velocity, -velocity_range, velocity_range
        )  # Due to precision errors in simulator, returned velocity may be slightly larger than lidar_range
        velocity /= velocity_range  # Normalize between -1.0 and 1.0

        # obtain angular velocity observation
        angular_velocity = self.engine.getTankAngularVelocity(id)
        angular_velocity_range = 9.84575  # Empirically determined

        angular_velocity = np.clip(
            angular_velocity, -angular_velocity_range, angular_velocity_range
        )
        angular_velocity /= angular_velocity_range  # Normalize between -1.0 and 1.0

        # obtain reload counter observation
        reload_counter = self.agent_data[agent].reload_counter
        reload_counter /= max(
            self.metadata["reload_delay"], 1
        )  # Normalize between 0.0 and 1.0

        # return observations
        observations = np.append(
            lidar_scan, (velocity[0], velocity[1], angular_velocity, reload_counter)
        ).astype(np.float32)

        return observations

    @functools.lru_cache(maxsize=None)
    def observation_space(self, agent):
        """Return agent's observation space.
        [0-359] - lidar (% range)
        [360] - local velocity X axis (% range)
        [361] - local velocity Y axis (% range)
        [362] - local angular velocity (% range)
        [363] - reload counter (% range)
        """
        lidar_points = self.tank_metadata["lidar_points"]

        return Box(-1.0, 1.0, shape=(lidar_points + 4,), dtype=np.float32)

    @functools.lru_cache(maxsize=None)
    def action_space(self, agent):
        """Return agent's action space.
        [0] - left tread (% speed)
        [1] - right tread (% speed)
        [2] - fire gun (if positive)
        """

        return Box(-1.0, 1.0, shape=(3,), dtype=np.float32)

    def close(self):
        """Uninitialize components."""

        if self.screen is not None:
            pygame.quit()
            self.screen = None
