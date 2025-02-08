# Tank Game (@kennedyengineering)

from .. import tank_game_environment_v0

from ..agent.agent_random import RandomAgent
from ..agent.agent_static import StaticAgent

env = tank_game_environment_v0.parallel_env_fn(render_mode="human")
observations, infos = env.reset()

assert len(env.agents) == 2
random_agent = RandomAgent(
    env.observation_space(env.agents[0]), env.action_space(env.agents[0])
)
static_agent = StaticAgent(
    env.observation_space(env.agents[1]),
    env.action_space(env.agents[1]),
    [0.25, 0.5, 0],
)

while env.agents:
    actions = {
        env.agents[0]: random_agent.policy(observations[env.agents[0]]),
        env.agents[1]: static_agent.policy(observations[env.agents[1]]),
    }

    observations, rewards, terminations, truncations, infos = env.step(actions)

env.close()
