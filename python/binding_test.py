import sys
import os

# Add the build directory to the sys.path
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '../build')))

import python_bindings
from PIL import Image

print("Module", dir(python_bindings))

config = python_bindings.Config()
print("Config", dir(config))

engine = python_bindings.Engine(config)
print("Engine", dir(engine))

tankConfig = python_bindings.TankConfig()
tankConfig.positionX = 20
tankConfig.positionY = 20
tankConfig.angle = 0.6
print("TankConfig", dir(tankConfig))

tankId1 = engine.addTank(tankConfig)
print("TankId", tankId1)

tankId2 = engine.addTank(tankConfig)
print("TankId", tankId2)

engine.removeTank(tankId1)

engine.renderTank(tankId2)

image = engine.getImage()
print("Image Shape", image.shape)

pil_image = Image.fromarray(image)
pil_image.save("output.png")
