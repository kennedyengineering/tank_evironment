import sys
import os

# Add the build directory to the sys.path
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '../build')))

import tank_game
import numpy as np
from PIL import Image

print(dir(tank_game))

print(tank_game.engineInit())

action = tank_game.TankAction()
action.gun_angle = 0
action.fire_gun = False
action.left_tread_force = 0
action.right_tread_force = 0

print(tank_game.engineStep(action, action))

frame = tank_game.engineRender()
print(frame)
print(frame.shape)

im = Image.fromarray(frame)
im.save("frame.png")

print(tank_game.engineDestroy())
