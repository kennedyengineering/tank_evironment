import torch
import torch.nn as nn
import torch.nn.functional as F

from gymnasium import spaces

from stable_baselines3.common.torch_layers import BaseFeaturesExtractor


class CausalDilatedConv2d(nn.Module):
    def __init__(self, in_channels, out_channels, kernel_size, dilation=(1, 1)):
        """
        Custom 2D convolution that applies:
          - Zero padding on the time dimension for causality.
          - Circular padding on the angular dimension.
        Parameters:
            in_channels (int): number of input channels.
            out_channels (int): number of output channels.
            kernel_size (tuple): (kernel_time, kernel_angle).
            dilation (tuple): (dilation_time, dilation_angle).
        """
        super(CausalDilatedConv2d, self).__init__()

        # The convolution layer (without built-in padding)
        self.conv = nn.Conv2d(
            in_channels,
            out_channels,
            kernel_size=kernel_size,
            dilation=dilation,
            bias=False,
        )

        # Compute padding amounts.
        # For the time dimension (causal, zero padding): pad only at the beginning.
        self.pad_time = dilation[0] * (kernel_size[0] - 1)

        # For the angular dimension (circular): pad equally on both sides.
        self.pad_angle = dilation[1] * (kernel_size[1] - 1) // 2

    def forward(self, x):
        # x shape: (batch_size, channels, time, angle)

        # Step 1: Zero padding for time.
        x = F.pad(x, (0, 0, self.pad_time, 0), mode="constant", value=0)

        # Step 2: Circular padding for angle.
        x = F.pad(x, (self.pad_angle, self.pad_angle, 0, 0), mode="circular")

        # Apply the convolution.
        return self.conv(x)


class TCNBlock(nn.Module):
    def __init__(self, in_channels, out_channels, kernel_size, dilation, dropout=0.0):
        """
        A single TCN block using our custom causal dilated convolution.
        Includes:
          - Causal dilated convolution.
          - ReLU non-linearity.
          - Dropout.
          - Residual connection.
        """
        super(TCNBlock, self).__init__()
        self.conv = CausalDilatedConv2d(
            in_channels, out_channels, kernel_size, dilation
        )
        self.relu = nn.ReLU()
        self.dropout = nn.Dropout(dropout)

        # If in_channels != out_channels, adjust with a 1x1 convolution for the residual.
        self.downsample = None
        if in_channels != out_channels:
            self.downsample = nn.Conv2d(in_channels, out_channels, kernel_size=1)

    def forward(self, x):
        out = self.conv(x)
        out = self.relu(out)
        out = self.dropout(out)

        # Residual connection.
        res = x if self.downsample is None else self.downsample(x)
        return self.relu(out + res)


class LidarTCN(nn.Module):
    def __init__(
        self,
        in_channels=1,
        num_channels=[16, 32, 64, 64],
        kernel_size=(3, 3),
        dilation_base=2,
        features=128,
        dropout=0.1,
    ):
        """
        A temporal convolutional network for compressing lidar history.
        Parameters:
            in_channels (int): Number of input channels (e.g. 1 for your (64,360) data).
            num_channels (list): A list specifying the number of channels in each TCN layer.
            kernel_size (tuple): Kernel size for each block.
            dropout (float): Dropout probability.
        """
        super(LidarTCN, self).__init__()

        layers = []
        num_layers = len(num_channels)
        # The dilation in time doubles each layer.
        for i in range(num_layers):
            # The first layer takes in_channels, subsequent layers use the previous block's output.
            in_ch = in_channels if i == 0 else num_channels[i - 1]
            out_ch = num_channels[i]
            # Dilation: double the temporal dilation factor at each layer; angle dilation is 1.
            dilation = (dilation_base**i, 1)
            layers.append(TCNBlock(in_ch, out_ch, kernel_size, dilation, dropout))

        self.network = nn.Sequential(*layers)

        # Final compression layer: Global average pooling over time and angle,
        # then a fully connected layer to produce a compressed feature vector.
        self.global_pool = nn.AdaptiveAvgPool2d((1, 1))
        self.fc = nn.Linear(num_channels[-1], features)  # e.g., output feature size 128

    def forward(self, x):
        # x shape: (batch_size, 1, time, angle) e.g. (B, 1, 64, 360)
        x = self.network(x)
        # out shape: (B, num_channels[-1], T, A)

        # Global average pooling
        x = self.global_pool(x)  # shape: (B, num_channels[-1], 1, 1)
        x = x.view(x.size(0), -1)  # shape: (B, num_channels[-1])

        # Final fully connected layer
        x = self.fc(x)  # shape: (B, 128)
        return x


class TemporalLidarCNN(BaseFeaturesExtractor):
    def __init__(
        self,
        observation_space: spaces.Box,
        num_channels=[16, 32, 32],
        length=50,
        kernel_size=(5, 5),
        dilation_base=3,
        features=128,
        dropout=0.1,
    ):
        self.length = length
        super().__init__(observation_space, features + 4)

        # Process lidar features through the temporal convolutional network
        self.lidar_tcn = LidarTCN(
            in_channels=1,
            num_channels=num_channels,
            kernel_size=kernel_size,
            dilation_base=dilation_base,
            features=features,
            dropout=dropout,
        )

        # Process extra features through a small network to normalize them.
        self.extra_fc = nn.Sequential(nn.Linear(4, 16), nn.ReLU(), nn.Linear(16, 4))

    def forward(self, observations: torch.Tensor) -> torch.Tensor:

        # Reshape observations
        observations = observations.view(-1, self.length, 364)

        # Process LIDAR data (assumed to be the first 360 values)
        x = observations[:, :, :360]  # shape: (batch, -1, 360)
        x = x.unsqueeze(1)  # shape: (batch, 1, -1, 360)
        cnn_features = self.lidar_tcn(x)

        # Process extra features (assumed to be the last 4 values)
        x = observations[:, 0, 360:]
        extra_features = self.extra_fc(x)  # Shape: (batch, -1, 4)

        # Concatenate processed CNN features with extra features.
        return torch.cat([cnn_features, extra_features], dim=1)
