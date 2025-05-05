# Tank Game (@kennedyengineering)

from ..map_base import MapData, TankMapData, ArenaMapData
from ..map_registry import register_map

import numpy as np
from dataclasses import dataclass, field


@register_map
@dataclass
class RunCloseRTL(MapData):
    "Right-to-Left variant"

    arena_map_data: ArenaMapData = field(
        default_factory=lambda: ArenaMapData(height=100, width=100)
    )

    # tank metadata
    tank_count: int = 2
    tank_map_data: list[TankMapData] = field(
        default_factory=lambda: [
            TankMapData(position_x=50, position_y=85, angle=-np.pi),
            TankMapData(position_x=85, position_y=65, angle=np.pi / 2),
        ]
    )


@register_map
@dataclass
class RunCloseLTR(MapData):
    "Left-to-Right variant"

    arena_map_data: ArenaMapData = field(
        default_factory=lambda: ArenaMapData(height=100, width=100)
    )

    # tank metadata
    tank_count: int = 2
    tank_map_data: list[TankMapData] = field(
        default_factory=lambda: [
            TankMapData(position_x=50, position_y=85, angle=-np.pi),
            TankMapData(position_x=15, position_y=65, angle=-np.pi / 2),
        ]
    )
