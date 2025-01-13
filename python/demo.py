# Import python_bindings
import sys
import os

# Add the build directory to the sys.path
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), "../build")))

import python_bindings as tank_game

# Import other packages
from contextlib import redirect_stdout

with redirect_stdout(None):
    import pygame

import numpy as np

# Initialize engine
config = tank_game.Config()
engine = tank_game.Engine(config)

# Initialize tanks
player_tank_config = tank_game.TankConfig()
player_tank_config.positionX = config.arenaWidth / 3
player_tank_config.positionY = config.arenaHeight / 2

player_tank_id = engine.addTank(player_tank_config)

other_tank_config = tank_game.TankConfig()
other_tank_config.positionX = 2 * config.arenaWidth / 3
other_tank_config.positionY = config.arenaHeight / 2
other_tank_config.angle = np.pi

other_tank_id = engine.addTank(other_tank_config)

# Initialize Pygame
pygame.init()

# Set up the window size
width, height = engine.getImageDimensions()
screen = pygame.display.set_mode((width, height))
pygame.display.set_caption("Tank Game Demo")

# Initialize clock for FPS management
clock = pygame.time.Clock()

# Font for displaying FPS
font = pygame.font.SysFont(None, 36)

# Game loop
running = True
move_gun_accumulator = 0
while running:

    # Get input
    movement_force = 3000.0
    move_left_tread = 0
    move_right_tread = 0

    movement_angle = 0.06
    move_gun = 0

    fire_projectile = False

    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        if event.type == pygame.KEYDOWN:
            if event.key == pygame.K_SPACE:
                fire_projectile = True

    keys = pygame.key.get_pressed()
    if keys[pygame.K_q]:
        move_left_tread += movement_force
    if keys[pygame.K_a]:
        move_left_tread -= movement_force
    if keys[pygame.K_e]:
        move_right_tread += movement_force
    if keys[pygame.K_d]:
        move_right_tread -= movement_force
    if keys[pygame.K_z]:
        move_gun -= movement_angle
    if keys[pygame.K_x]:
        move_gun += movement_angle

    # Apply input
    if move_left_tread:
        engine.moveLeftTankTread(player_tank_id, move_left_tread)
    if move_right_tread:
        engine.moveRightTankTread(player_tank_id, move_right_tread)

    if move_gun:
        move_gun_accumulator += move_gun
        engine.rotateTankGun(player_tank_id, move_gun_accumulator)

    if fire_projectile:
        engine.fireTankGun(player_tank_id)

    # Step the simulation
    engine.step()

    # Render scene
    engine.clearImage()

    engine.renderTank(player_tank_id)
    engine.renderTank(other_tank_id)

    engine.scanTankLidar(player_tank_id)
    engine.renderTankLidar(player_tank_id)

    engine.renderProjectiles()

    # Display image
    image_buff = engine.getImageBuffer()  # Get buffer
    image_buff = np.swapaxes(image_buff, 0, 1)  # Swap axis
    image = pygame.surfarray.make_surface(image_buff)
    screen.blit(image, (0, 0))

    # Display FPS
    fps = clock.get_fps()  # Get the current FPS
    fps_text = font.render(f"FPS: {fps:.2f}", True, (255, 0, 0))  # Render FPS text
    screen.blit(fps_text, (10, 10))  # Draw FPS on the screen

    # Update the display
    pygame.display.flip()

    # Cap the frame rate
    clock.tick(30)

# Quit Pygame
pygame.quit()
sys.exit()
