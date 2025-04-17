# Tank Game (@kennedyengineering)

from .. import tank_game_environment_v0

env = tank_game_environment_v0.parallel_env_fn(
    render_mode="human", map_id="GridStaggered"
)
observations, infos = env.reset()

while env.agents:
    actions = {agent: env.action_space(agent).sample() for agent in env.agents}

    observations, rewards, terminations, truncations, infos = env.step(actions)

env.close()
