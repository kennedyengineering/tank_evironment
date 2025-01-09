import sys
import os

# Add the build directory to the sys.path
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '../build')))

import tank_game

print("Module", dir(tank_game))

config = tank_game.Config()
print("Config", dir(config))

engine = tank_game.Engine(config)
print("Engine", dir(engine))

tankConfig = tank_game.TankConfig()
print("TankConfig", dir(tankConfig))

tankId1 = engine.addTank(tankConfig)
print("TankId", tankId1)

tankId2 = engine.addTank(tankConfig)
print("TankId", tankId2)

engine.removeTank(tankId1)

image = engine.getImage()
print("Image Shape", image.shape)
