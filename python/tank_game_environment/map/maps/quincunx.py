# Tank Game (@kennedyengineering)

from ..map_base import MapData, TankMapData, ObstacleMapData, ArenaMapData
from ..map_registry import register_map

import numpy as np
from dataclasses import dataclass, field


@register_map
@dataclass
class Quincunx(MapData):
    arena_map_data: ArenaMapData = field(
        default_factory=lambda: ArenaMapData(height=100, width=100)
    )

    # tank metadata
    tank_count: int = 2
    tank_map_data: list[TankMapData] = field(
        default_factory=lambda: [
            TankMapData(position_x=15, position_y=50, angle=-np.pi / 2),
            TankMapData(position_x=85, position_y=50, angle=np.pi / 2),
        ]
    )

    # obstacle metadata
    obstacle_map_data: list[ObstacleMapData] = field(
        default_factory=lambda: [
            ObstacleMapData(position_x=75, position_y=25, radius=10),
            ObstacleMapData(position_x=75, position_y=75, radius=10),
            ObstacleMapData(position_x=50, position_y=50, radius=10),
            ObstacleMapData(position_x=25, position_y=75, radius=10),
            ObstacleMapData(position_x=25, position_y=25, radius=10),
        ]
    )
