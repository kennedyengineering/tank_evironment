import sys
import os

# Add the build directory to the sys.path
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '../build')))

import tank_game

print(dir(tank_game))

config = tank_game.Config()
print(dir(config))

engine = tank_game.Engine(config)
print(dir(engine))
