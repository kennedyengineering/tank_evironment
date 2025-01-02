import sys
import os

# Add the build directory to the sys.path
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '../build')))

import tank_game
import numpy as np
from PIL import Image
import time

print(dir(tank_game))

print(tank_game.engineInit())

action = tank_game.TankAction()
action.gun_angle = 10
action.fire_gun = False
action.left_tread_force = 500
action.right_tread_force = 0

print(tank_game.engineStep(action, action))

frame = tank_game.engineRender()
print(frame)
print(frame.shape)

steps = 500
start = time.time()
for i in range(steps):
    tank_game.engineStep(action, action)
    tank_game.engineRender()
stop = time.time()

print("Time taken: ", stop - start)
print("FPS: ", steps / (stop - start))

im = Image.fromarray(frame)
im.save("frame.png")

print(tank_game.engineDestroy())
