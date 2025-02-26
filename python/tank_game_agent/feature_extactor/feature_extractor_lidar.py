# Tank Game (@kennedyengineering)

# adapted from https://stable-baselines3.readthedocs.io/en/master/guide/custom_policy.html#custom-feature-extractor

import torch as th
import torch.nn as nn
import torch.nn.functional as F
from gymnasium import spaces

from stable_baselines3.common.torch_layers import BaseFeaturesExtractor


class LidarCNN(BaseFeaturesExtractor):
    def __init__(self, observation_space: spaces.Box, features_dim: int = 10):
        super().__init__(observation_space, features_dim + 4)
        input_size = observation_space.shape[0] - 4

        # Define convolutional layers with no built-in padding
        self.conv1 = nn.Conv1d(
            in_channels=1, out_channels=16, kernel_size=5, stride=1, padding=0
        )
        self.conv2 = nn.Conv1d(
            in_channels=16, out_channels=32, kernel_size=5, stride=1, padding=0
        )

        # Calculate new length after two convolutions (length remains the same if we pad correctly)
        # For kernel_size 5, we need a padding of 2 on each side
        self.fc1 = nn.Linear(32 * input_size, 128)
        self.fc2 = nn.Linear(128, features_dim)

    def forward(self, observations: th.Tensor) -> th.Tensor:

        # reshape input
        x = observations[:, :360].unsqueeze(1)

        # Apply circular padding manually before each conv layer.
        # For kernel_size=5, pad with 2 elements on each side.
        x = F.pad(x, (2, 2), mode="circular")
        x = F.relu(self.conv1(x))

        x = F.pad(x, (2, 2), mode="circular")
        x = F.relu(self.conv2(x))

        # Flatten the output for the fully connected layer.
        x = x.view(x.size(0), -1)
        x = F.relu(self.fc1(x))
        x = self.fc2(x)

        return th.cat([x, observations[:, 360:]], dim=1)
