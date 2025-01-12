# Import python_bindings
import sys
import os

# Add the build directory to the sys.path
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), "../build")))

import python_bindings as tank_game

# Import other packages
import pygame
import numpy as np

# Initialize engine
config = tank_game.Config()
engine = tank_game.Engine(config)

# Initialize tanks
tank_config = tank_game.TankConfig()
tank_config.positionX = config.arenaWidth / 2
tank_config.positionY = config.arenaHeight / 2

tank_id = engine.addTank(tank_config)

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
while running:

    # Get input
    movement_force = 3000.0
    move_left_tread = 0
    move_right_tread = 0

    fire_projectile = False

    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        if event.type == pygame.KEYDOWN:
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

    # Apply input
    movement_force = 3000.0
    if move_left_tread:
        engine.moveLeftTankTread(tank_id, move_left_tread)
    if move_right_tread:
        engine.moveRightTankTread(tank_id, move_right_tread)

    if fire_projectile:
        engine.fireTankGun(tank_id)

    # Step the simulation
    engine.step()

    # Render scene
    engine.clearImage()

    engine.renderTank(tank_id)

    engine.scanTankLidar(tank_id)
    engine.renderTankLidar(tank_id)

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
    clock.tick(60)

# Quit Pygame
pygame.quit()
sys.exit()
