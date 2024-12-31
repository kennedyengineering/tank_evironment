import sys
import os

# Add the build directory to the sys.path
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '../build')))

import tank_game

print(dir(tank_game))

print(tank_game.engineInit())

action = tank_game.TankAction()
action.gun_angle = 0
action.fire_gun = False
action.left_tread_force = 0
action.right_tread_force = 0

print(tank_game.engineStep(action, action))

print(tank_game.engineDestroy())
