
# Tank Game Environment

![](.github/screenshot.png)

## Goal
This project aims to develop a deep reinforcement learning agent skilled enough to beat human opponents in a 2D tank battle simulation.

## Methodology
The 'Tank Game' consists of two tanks attempting damage the other by firing projectiles.

Each tank is equipped with a lidar scanner that provides 360° distance measurements.

Each tank is capable of movement by adjusting power delivered to the tread motors, and is able to position it's gun by adjusting power delivered to the turret motor.

## Future Work
- Adding random obstacles
- Allow for more than two tanks
- Allow for tanks to be grouped into teams

## Repository Contents

## Installation

### WSL2 Notes
- Starting docker daemon in WSL2 cannot be done inside VSCode Terminal. Open a different terminal and enter `sudo service docker start`.
- Using development container expects Docker to be installed on Windows. To use the docker installed in WSL2, set `Dev › Containers: Execute In WSL` to `true` in `Settings`.
