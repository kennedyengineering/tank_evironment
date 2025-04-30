import gymnasium as gym
import torch
import numpy as np

from tank_game_environment.wrapper.wrapper_observation import ObservationWrapper


class FeatureExtractorWrapper(ObservationWrapper):
    def __init__(self, env, feature_extractor):
        super().__init__(env)
        self.feature_extractor = feature_extractor

        # Infer the new observation space using a dummy observation.
        dummy_obs = env.observation_space(None).sample()
        dummy_tensor = torch.tensor(dummy_obs, dtype=torch.float32)
        with torch.no_grad():
            # Add batch dimension: shape (1, *obs_shape)
            dummy_feat = self.feature_extractor(dummy_tensor.unsqueeze(0))
        # Remove batch dimension and convert to numpy array.
        dummy_feat = dummy_feat.squeeze(0).numpy()

        # Update the observation space based on the output of the feature extractor.
        self._observation_space = gym.spaces.Box(
            low=-np.inf, high=np.inf, shape=dummy_feat.shape, dtype=np.float32
        )

    def observation(self, observation):
        # Convert observation to tensor.
        obs_tensor = torch.tensor(observation, dtype=torch.float32)
        with torch.no_grad():
            # Process the observation (adding batch dimension).
            feat = self.feature_extractor(obs_tensor.unsqueeze(0))
        # Remove batch dimension and convert back to numpy.
        return feat.squeeze(0).numpy()

    def observation_space(self, agent):
        # Report the new feature-space
        return self._observation_space
