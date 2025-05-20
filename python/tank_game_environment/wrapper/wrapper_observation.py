from pettingzoo.utils.wrappers import BaseParallelWrapper

from gymnasium.logger import warn


class ObservationWrapper(BaseParallelWrapper):
    """
    A basic observation-only wrapper for Parallel PettingZoo envs.
    Override observation(self, obs) to transform each agent's obs.
    Override observation_space(self, agent) to transform an agent's obs space.
    """

    def __init__(self, env):
        super().__init__(env)

    def reset(self, seed=None, options=None):
        """
        Reset the environment and wrap the initial observations.
        """
        observations, infos = self.env.reset(seed=seed, options=options)
        return self._wrap_obs(observations), infos

    def step(self, actions):
        """
        Step the environment and wrap the resulting observations.
        """
        observations, rewards, terminations, truncations, infos = self.env.step(actions)
        return self._wrap_obs(observations), rewards, terminations, truncations, infos

    def _wrap_obs(self, obs_dict):
        """
        Apply self.observation to each agent's observation.
        """
        return {agent: self.observation(agent, o) for agent, o in obs_dict.items()}

    def observation(self, agent, observation):
        """
        Override this method to transform a single agent's observation.
        By default, returns it unchanged.
        """

        warn("ObservationWrapper `observation(self, obs)` method is default.")

        return observation

    def observation_space(self, agent):
        """
        By default, assume the wrapper doesn't change the space.
        Subclasses *must* override this if they reshape/modify obs!
        """

        warn("ObservationWrapper `observation_space(self, agent)` method is default.")

        return self.env.observation_space(agent)
