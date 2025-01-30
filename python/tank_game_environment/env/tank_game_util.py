# Tank Game (@kennedyengineering)

import python_bindings as tank_game

from dataclasses import dataclass


@dataclass
class TankData:
    id: int = -1
    config: tank_game.TankConfig = tank_game.TankConfig()
