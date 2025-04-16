# Tank Game (@kennedyengineering)

from dataclasses import dataclass, field
from math import pi

# TODO: replace maps/ directory with config files?


@dataclass
class ArenaMapData:
    """Holds information about the arena dimensions in manually crafted arena configurations"""

    height: float
    width: float


@dataclass
class TankMapData:
    """Holds information about a tank in manually crafted arena configurations"""

    position_x: float
    position_y: float

    angle: float


@dataclass
class ObstacleMapData:
    """Holds information about an obstacle in manually crafted arena configurations"""

    position_x: float
    position_y: float

    radius: float


@dataclass
class MapData:
    """Holds information about manually crafted arena configurations"""

    arena_map_data: ArenaMapData

    tank_count: int
    tank_map_data: list[TankMapData] = field(default_factory=list)

    obstacle_map_data: list[ObstacleMapData] = field(default_factory=list)

    def __post_init__(self):
        # Validate arena configuration
        assert self.arena_map_data.height > 0, "Invalid arena height"
        assert self.arena_map_data.width > 0, "Invalid arena width"

        # Validate tank configuration
        for tank_map_data in self.tank_map_data:
            assert (
                0 <= tank_map_data.position_x <= self.arena_map_data.width
            ), "Invalid tank position X"
            assert (
                0 <= tank_map_data.position_y <= self.arena_map_data.height
            ), "Invalid tank position Y"
            assert -2 * pi <= tank_map_data.angle <= 2 * pi, "Invalid tank angle"

        assert self.tank_count == len(self.tank_map_data), "Incorrect tank count"

        # Validate obstacle configuration
        for obstacle_map_data in self.obstacle_map_data:
            assert (
                0 <= obstacle_map_data.position_x <= self.arena_map_data.width
            ), "Invalid obstacle position X"
            assert (
                0 <= obstacle_map_data.position_y <= self.arena_map_data.height
            ), "Invalid obstacle position Y"
            assert 0 < obstacle_map_data.radius, "Invalid obstacle radius"

    @classmethod
    def get_num_tanks(cls):
        return cls.tank_count
