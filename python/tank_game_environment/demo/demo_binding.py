# Tank Game (@kennedyengineering)

import python_bindings as tank_game

from contextlib import redirect_stdout

with redirect_stdout(None):
    import pygame

import numpy as np

# Initialize engine
config = tank_game.Config()
config.verboseOutput = True
engine = tank_game.Engine(config)

# Initialize obstacles
obstacle_config_0 = tank_game.ObstacleConfig()
obstacle_config_0.positionX = config.arenaWidth / 2
obstacle_config_0.positionY = config.arenaHeight / 2
obstacle_config_0.radius = 10

obstacle_id_0 = engine.addObstacle(obstacle_config_0)

obstacle_config_1 = tank_game.ObstacleConfig()
obstacle_config_1.positionX = 5
obstacle_config_1.positionY = 5
obstacle_config_1.radius = 5

obstacle_id_1 = engine.addObstacle(obstacle_config_1)

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
    movement_speed = 15.0
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
        move_left_tread += movement_speed
    if keys[pygame.K_a]:
        move_left_tread -= movement_speed
    if keys[pygame.K_e]:
        move_right_tread += movement_speed
    if keys[pygame.K_d]:
        move_right_tread -= movement_speed
    if keys[pygame.K_z]:
        move_gun -= movement_angle
    if keys[pygame.K_x]:
        move_gun += movement_angle

    # Apply input
    engine.moveLeftTankTread(player_tank_id, move_left_tread)
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

    engine.renderObstacle(obstacle_id_0)
    engine.renderObstacle(obstacle_id_1)

    engine.renderTank(player_tank_id)
    engine.renderTank(other_tank_id)

    engine.scanTankLidar(player_tank_id)
    engine.renderTankLidar(player_tank_id)

    engine.renderProjectiles()

    # Display image
    image_buff = engine.getImageBuffer()  # Get buffer
    image_buff = np.swapaxes(image_buff, 0, 1)  # Swap axis
    image = pygame.surfarray.make_surface(image_buff)

    pos = engine.getTankPosition(player_tank_id)
    vel_w = engine.getTankWorldVelocity(player_tank_id)
    vel_l = engine.getTankLocalVelocity(player_tank_id)
    vel_a = engine.getTankAngularVelocity(player_tank_id)
    a = engine.getTankOrientation(player_tank_id)

    start_pixels = [config.pixelDensity * x for x in pos]

    end_pixels = [
        config.pixelDensity * x + start_pixels[i] for i, x in enumerate(vel_w)
    ]
    pygame.draw.line(image, (255, 0, 0), start_pixels, end_pixels)

    # end_pixels = [
    #     config.pixelDensity * x + start_pixels[i] for i, x in enumerate(vel_l)
    # ]
    # pygame.draw.line(image, (0, 255, 0), start_pixels, end_pixels)

    arc_radius = 10
    arc_radius_pixels = arc_radius * config.pixelDensity
    arc_rect = (
        start_pixels[0] - arc_radius_pixels,
        start_pixels[1] - arc_radius_pixels,
        arc_radius_pixels * 2,
        arc_radius_pixels * 2,
    )
    if vel_a < 0:
        pygame.draw.arc(image, (0, 0, 255), arc_rect, vel_a / 2 - a, -a)
    else:
        pygame.draw.arc(image, (0, 0, 255), arc_rect, -a, vel_a / 2 - a)

    screen.blit(image, (0, 0))

    # Display FPS
    # fps = clock.get_fps()  # Get the current FPS
    # fps_text = font.render(f"FPS: {fps:.2f}", True, (255, 0, 0))  # Render FPS text
    # screen.blit(fps_text, (10, 10))  # Draw FPS on the screen

    # Update the display
    pygame.display.flip()

    # Cap the frame rate
    clock.tick(30)

# Quit Pygame
pygame.quit()
