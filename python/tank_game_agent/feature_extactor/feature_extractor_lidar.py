# Tank Game (@kennedyengineering)

# adapted from https://stable-baselines3.readthedocs.io/en/master/guide/custom_policy.html#custom-feature-extractor

import torch as th
import torch.nn as nn
import torch.nn.functional as F
from gymnasium import spaces

from stable_baselines3.common.torch_layers import BaseFeaturesExtractor


class LidarCNN(BaseFeaturesExtractor):
    def __init__(
        self,
        observation_space: spaces.Box,
        conv_features_count: int = 128,
        extra_feature_count: int = 4,
        extra_observations: int = 4,
        channels: int = 4,
    ):
        """
        Supports observations that are either flat vectors (e.g., (364,))
        or channel-first images (e.g., (4, 364)).

        In both cases, we assume:
         - The last N elements (or columns) are extra features.
         - The remaining values are LIDAR readings.
        """

        # Final output is the concatenation of CNN features and processed extra features.
        super().__init__(observation_space, conv_features_count + extra_feature_count)

        # We assume the observation vector is divisble by the number of channels.
        total_length = observation_space.shape[-1] // channels

        # We assume the extra features are in the last 4 values/columns.
        lidar_length = total_length - extra_observations

        self.channels = channels
        self.total_length = total_length
        self.lidar_length = lidar_length
        self.extra_feature_count = extra_feature_count

        # Two rounds of pooling reduce the length by a factor of 4.
        final_length = lidar_length // 4

        # Convolutional Block for LIDAR.
        # Input shape will be (batch, channels, lidar_length)
        self.conv1 = nn.Conv1d(
            in_channels=channels, out_channels=16, kernel_size=5, stride=1, padding=0
        )
        self.bn1 = nn.BatchNorm1d(16)
        self.pool1 = nn.MaxPool1d(kernel_size=2)

        self.conv2 = nn.Conv1d(
            in_channels=16, out_channels=32, kernel_size=5, stride=1, padding=0
        )
        self.bn2 = nn.BatchNorm1d(32)
        self.pool2 = nn.MaxPool1d(kernel_size=2)

        self.dropout = nn.Dropout(0.5)

        # Fully connected layers for the CNN branch.
        self.fc1 = nn.Linear(32 * final_length, 128)
        self.fc2 = nn.Linear(128, conv_features_count)

        # Process extra features.
        # In the flat case extra features dimension is 4.
        # In the multi-dimensional case, extra features have shape (channels, 4)
        # which we flatten to channels*4.
        extra_dim = channels * extra_feature_count
        self.extra_fc = nn.Sequential(
            nn.Linear(extra_dim, 16), nn.ReLU(), nn.Linear(16, extra_feature_count)
        )

    def forward(self, observations: th.Tensor) -> th.Tensor:
        """
        Process the observation
        """

        # Reshape observations
        observations = observations.view(-1, self.channels, self.total_length)
        # observations: (batch, channels, 364)
        lidar = observations[:, :, : self.lidar_length]  # (batch, channels, 360)
        extra = observations[:, :, self.lidar_length :]  # (batch, channels, 4)
        # Flatten extra features from (batch, channels, 4) to (batch, channels*4)
        extra = extra.flatten(1)

        # --- Convolutional Block 1 ---
        # Circular padding to preserve sequence length.
        x = F.pad(lidar, (2, 2), mode="circular")
        x = self.conv1(x)
        x = self.bn1(x)
        x = F.relu(x)
        x = self.pool1(x)  # reduces length by factor of 2

        # --- Convolutional Block 2 ---
        x = F.pad(x, (2, 2), mode="circular")
        x = self.conv2(x)
        x = self.bn2(x)
        x = F.relu(x)
        x = self.pool2(x)  # reduces length by another factor of 2

        # Flatten the CNN output.
        x = x.view(x.size(0), -1)
        x = self.dropout(x)
        x = F.relu(self.fc1(x))
        cnn_features = self.fc2(x)  # (batch, features_dim)

        # Process extra features.
        extra_features = self.extra_fc(extra)  # (batch, 4)

        # Concatenate processed CNN features with extra features.
        return th.cat([cnn_features, extra_features], dim=1)
