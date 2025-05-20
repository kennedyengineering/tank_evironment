# Tank Game (@kennedyengineering)

# adapted from https://stable-baselines3.readthedocs.io/en/master/guide/custom_policy.html#custom-feature-extractor

import torch as th
import torch.nn as nn
import torch.nn.functional as F
from gymnasium import spaces

from stable_baselines3.common.torch_layers import BaseFeaturesExtractor


class LidarCNN(BaseFeaturesExtractor):
    def __init__(self, observation_space: spaces.Box, features_dim: int = 10):

        # Final output dimension = features_dim (from CNN branch) + 4 (from extra features)
        super().__init__(observation_space, features_dim + 4)

        # Assume the last 4 are extra features
        input_size = observation_space.shape[0] - 4

        # Two rounds of pooling reduce the length by a factor of 4
        final_length = input_size // 4

        # Convolutional Block 1
        self.conv1 = nn.Conv1d(
            in_channels=1, out_channels=32, kernel_size=5, stride=1, padding=0
        )
        self.bn1 = nn.BatchNorm1d(32)
        self.pool1 = nn.MaxPool1d(kernel_size=2)

        # Convolutional Block 2
        self.conv2 = nn.Conv1d(
            in_channels=32, out_channels=64, kernel_size=5, stride=1, padding=0
        )
        self.bn2 = nn.BatchNorm1d(64)
        self.pool2 = nn.MaxPool1d(kernel_size=2)

        # Dropout for regularization.
        self.dropout = nn.Dropout(0.5)

        # Fully connected layers for the CNN branch.
        self.fc1 = nn.Linear(64 * final_length, 256)
        self.fc2 = nn.Linear(256, features_dim)

        # Process extra features through a small network to normalize them.
        self.extra_fc = nn.Sequential(nn.Linear(4, 16), nn.ReLU(), nn.Linear(16, 4))

    def forward_conv_preprocess(self, x):
        # Process LIDAR data (assumed to be the first 360 values)
        x = x[:, :360]  # shape: (batch, 360)
        x = x.unsqueeze(1)  # shape: (batch, 1, 360)
        return x

    def forward_conv_block1(self, x):
        # --- Convolutional Block 1 ---
        # Circular padding so that length remains the same.
        x = F.pad(x, (2, 2), mode="circular")
        x = self.conv1(x)
        x = self.bn1(x)
        x = F.relu(x)
        x = self.pool1(x)  # reduces length by factor of 2
        return x

    def forward_conv_block2(self, x):
        # --- Convolutional Block 2 ---
        # Circular padding so that length remains the same.
        x = F.pad(x, (2, 2), mode="circular")
        x = self.conv2(x)
        x = self.bn2(x)
        x = F.relu(x)
        x = self.pool2(x)  # reduces length further by factor of 2
        return x

    def forward_conv_features(self, x):
        # Flatten the CNN output.
        x = x.view(x.size(0), -1)
        x = self.dropout(x)
        x = F.relu(self.fc1(x))
        x = self.fc2(x)  # Shape: (batch, features_dim)
        return x

    def forward_extra_preprocess(self, x):
        # Process extra features (assumed to be the last 4 values)
        x = x[:, 360:]
        return x

    def forward_extra_mlp(self, x):
        x = self.extra_fc(x)  # Shape: (batch, 4)
        return x

    def forward(self, observations: th.Tensor) -> th.Tensor:

        # Lidar Head.
        x = self.forward_conv_preprocess(observations)
        x = self.forward_conv_block1(x)
        x = self.forward_conv_block2(x)
        cnn_features = self.forward_conv_features(x)

        # Extra features Head.
        x = self.forward_extra_preprocess(observations)
        extra_features = self.forward_extra_mlp(x)

        # Concatenate processed CNN features with extra features.
        return th.cat([cnn_features, extra_features], dim=1)
