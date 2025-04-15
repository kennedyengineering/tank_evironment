# Tank Game (@kennedyengineering)

from .map_base import MapData, TankMapData, ObstacleMapData, ArenaMapData
from .map_registry import register_map

import numpy as np
from dataclasses import dataclass, field
from gymnasium.logger import error

# TODO: non-obstacle version


@register_map
@dataclass
class RandomObstacles(MapData):
    arena_map_data: ArenaMapData = field(
        default_factory=lambda: ArenaMapData(height=100, width=100)
    )

    # tank metadata
    random_tank_count: int = 2

    # obstacle metadata
    max_random_obstacle_count: int = 5
    min_random_obstacle_count: int = 2

    min_random_obstacle_radius: float = 5.0
    max_random_obstacle_radius: float = 10.0

    # placement metadata
    max_placement_iterations: int = 10

    tank_wall_placement_buffer: float = (
        8.0  # minimum distance from wall to center of tank
    )
    tank_obstacle_placement_buffer: float = (
        8.0  # minimum distance from nearest edge of obstacle to center of tank
    )
    tank_tank_placement_buffer: float = (
        10.0  # minimum distance from center of tank to center of tank
    )

    obstacle_obstacle_gap: float = (
        8.0  # minimum "gap" / traversable distance between two obstacles
    )

    def __place_tank(self):
        """
        Randomly place tank on the map.

        Tanks are placed inside the arena walls and away from other tanks.

        Returns tank position, angle.
        """

        # place tank on the map
        placed = False
        for _ in range(self.max_placement_iterations):
            # generate random position away from arena walls
            position = np.random.rand(2) * [
                self.arena_map_data.width - self.tank_wall_placement_buffer * 2,
                self.arena_map_data.height - self.tank_wall_placement_buffer * 2,
            ]

            # place away from other tanks
            overlaps = False
            for tank_map_data in self.tank_map_data:
                # compute buffer distance
                distance = np.linalg.norm(
                    position - [tank_map_data.position_x, tank_map_data.position_y]
                )

                # enforce buffer distance
                if distance < self.tank_tank_placement_buffer:
                    overlaps = True
                    break

            # ensure buffer distance constraints are respected
            if not overlaps:
                placed = True
                break

        if not placed:
            error(
                f"Failed to place tank within {self.max_placement_iterations} iterations."
            )
            # TODO: handle error condition

        # generate random angle
        angle = np.random.rand() * 2 * np.pi

        return position, angle

    def __place_obstacle(self):
        """
        Randomly place obstacle on the map.

        Obstacles are placed away from other tanks and away from other obstacles.
        They may overlap with the arena walls.

        Returns obstacle position, radius.
        """

        # place obstacle on map
        placed = False
        for _ in range(self.max_placement_iterations):
            # generate random position
            position = np.random.rand(2) * [
                self.arena_map_data.width,
                self.arena_map_data.height,
            ]

            # generate random radius
            radius = (
                np.random.rand()
                * (self.max_random_obstacle_radius - self.min_random_obstacle_radius)
                + self.min_random_obstacle_radius
            )

            # place away from tanks
            overlaps = False
            for tank_map_data in self.tank_map_data:
                # compute buffer distance
                distance = np.linalg.norm(
                    position - [tank_map_data.position_x, tank_map_data.position_y]
                )
                distance -= radius

                # enforce buffer distance
                if distance < self.tank_obstacle_placement_buffer:
                    overlaps = True
                    break

            # skip subsequent checks if invalid placement
            if overlaps:
                continue

            # place away from obstacles
            overlaps = False
            for obstacle_map_data in self.obstacle_map_data:
                # compute gap distance
                distance = np.linalg.norm(
                    position
                    - [obstacle_map_data.position_x, obstacle_map_data.position_y]
                )
                distance -= radius + obstacle_map_data.radius

                # enforce gap distance
                if distance < self.obstacle_obstacle_gap:
                    overlaps = True
                    break

            # skip subsequent checks if invalid placement
            if overlaps:
                continue

            # all checks passed
            placed = True
            break

        if not placed:
            error(
                f"Failed to place obstacle within {self.max_placement_iterations} iterations."
            )
            # TODO: handle error condition

        return position, radius

    def __post_init__(self):
        # place tanks
        for _ in range(self.random_tank_count):
            position, angle = self.__place_tank()
            self.tank_map_data.append(
                TankMapData(position_x=position[0], position_y=position[1], angle=angle)
            )

        # place obstacles
        random_obstacle_count = np.random.randint(
            self.min_random_obstacle_count, self.max_random_obstacle_count + 1
        )
        for _ in range(random_obstacle_count):
            position, radius = self.__place_obstacle()
            self.obstacle_map_data.append(
                ObstacleMapData(
                    position_x=position[0], position_y=position[1], radius=radius
                )
            )

        # validate arena configuration
        super().__post_init__()
