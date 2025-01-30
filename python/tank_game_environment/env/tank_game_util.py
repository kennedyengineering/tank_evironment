# Tank Game (@kennedyengineering)

import python_bindings as tank_game

from dataclasses import dataclass


@dataclass
class TankData:
    id: int
    config: tank_game.TankConfig
