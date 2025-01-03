import sys
import os

# Add the build directory to the sys.path
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '../build')))

import tank_game

print("Module", dir(tank_game))

config = tank_game.Config()
print("Config", dir(config))

tankConfig = tank_game.TankConfig()
print("TankConfig", dir(tankConfig))

print(config.tankConfigs)
config.tankConfigs = [tankConfig]
print(config.tankConfigs)

engine = tank_game.Engine(config)
print("Engine", dir(engine))
