# Tank Game (@kennedyengineering)

import python_bindings as tank_game

from dataclasses import dataclass


@dataclass
class TankData:
    """
    Holds information about an agent.
    - id : engine assigned ID
    - iid : initialization ID
    - config : tank configuration struct
    """

    id: int
    iid: int
    config: tank_game.TankConfig
