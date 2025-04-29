import torch as th
from gymnasium import spaces
from stable_baselines3 import PPO
from stable_baselines3.common.torch_layers import BaseFeaturesExtractor


class FrozenFeatureExtractor(BaseFeaturesExtractor):
    """
    Wrap a trained SB3 policy's feature-extractor, freeze it, and
    expose it as a BaseFeaturesExtractor for a new model.
    """

    def __init__(
        self,
        observation_space: spaces.Box,
        *,
        checkpoint: str,
        device: str = "cpu",
    ):
        # Load donor model & grab its extractor (stand-alone module)
        donor = PPO.load(checkpoint, device=device)
        extractor = donor.policy.features_extractor.to(device)
        extractor.eval()
        for p in extractor.parameters():
            p.requires_grad = False

        # Compute output dimension from one dummy observation
        sample = observation_space.sample()
        tensor = th.as_tensor(sample, dtype=th.float32, device=device).unsqueeze(0)
        with th.no_grad():
            out = extractor(tensor)
        features_dim = out.shape[1]

        # Initialize the BaseFeaturesExtractor (and nn.Module)
        super().__init__(observation_space, features_dim)

        # Now it's safe to attach the sub-module
        self.extractor = extractor

    def forward(self, observations: th.Tensor) -> th.Tensor:
        # observations is already a float32 Tensor on the right device
        # just delegate to the frozen extractor
        with th.no_grad():
            return self.extractor(observations)
