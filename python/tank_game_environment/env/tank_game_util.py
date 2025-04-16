# Tank Game (@kennedyengineering)

from dataclasses import dataclass


@dataclass
class TankData:
    """
    Holds information about an agent.
    - id : engine assigned ID
    - reload_counter : number of steps until tank can fire
    """

    id: int
    reload_counter: int
