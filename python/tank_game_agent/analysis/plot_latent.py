import numpy as np
import matplotlib.pyplot as plt
from numpy.typing import NDArray
import itertools


def plot_conv(data: NDArray[np.float64]) -> None:
    """
    Plots each 1D convolutional filter's output as a heatmap in a tight grid.
    No titles, no ticks, no colorbar for minimal space usage.

    Assumes data has shape [time, filter_number, features].
    """

    # Extract dimensions
    n_time, n_filters, n_features = data.shape

    # Decide grid shape (roughly square)
    n_cols = int(np.ceil(np.sqrt(n_filters)))
    n_rows = int(np.ceil(n_filters / n_cols))

    # Create figure & subplots
    fig, axes = plt.subplots(
        n_rows, n_cols, figsize=(2 * n_cols, 2 * n_rows), sharex=True, sharey=True
    )

    # Convert axes to 2D array if needed
    if n_rows == 1 and n_cols == 1:
        axes = np.array([[axes]])
    elif n_rows == 1 or n_cols == 1:
        axes = np.array(axes, ndmin=2)

    # Remove extra spacing around/between subplots
    plt.subplots_adjust(
        wspace=0.05, hspace=0.05, left=0.05, right=0.98, top=0.98, bottom=0.05
    )

    for f_idx in range(n_filters):
        ax = axes[f_idx // n_cols, f_idx % n_cols]
        # Extract filter data and transpose
        plot_data = data[:, f_idx, :].T

        # Show heatmap
        ax.imshow(plot_data, cmap="viridis", origin="lower", aspect="auto")

        # Remove axis ticks and labels for a cleaner look
        ax.set_xticks([])
        ax.set_yticks([])

    # Turn off any unused subplots
    for idx in range(n_filters, n_rows * n_cols):
        axes[idx // n_cols, idx % n_cols].axis("off")

    plt.show()


def plot_latent(data: NDArray[np.float64]) -> None:
    # Determine the number of latent features (columns)
    n_latents = data.shape[1]

    # Create subplots dynamically. Adjust the figure height based on the number of latent features.
    fig, axes = plt.subplots(n_latents, 1, figsize=(12, 3 * n_latents), sharex=True)

    # Ensure axes is iterable when there's only one latent feature
    if n_latents == 1:
        axes = [axes]

    # Create a cycle of colors. This list can be modified or extended as needed.
    colors = itertools.cycle(
        ["blue", "green", "red", "purple", "orange", "brown", "pink", "gray"]
    )

    # Loop over each latent feature and plot it.
    for i in range(n_latents):
        color = next(colors)
        axes[i].plot(data[:, i], color=color, linewidth=2)
        axes[i].set_title(f"Latent {i+1}")
        axes[i].set_ylabel("Activation")
        axes[i].grid(True)

        # Only add the x-label to the bottom subplot for clarity.
        if i == n_latents - 1:
            axes[i].set_xlabel("Time")

    plt.tight_layout()
    plt.show()


def plot_latent_heatmap(data: NDArray[np.float64]) -> None:
    # Transpose the data so that each latent dimension becomes a row.
    # Original shape is assumed to be (time, latent)
    data_T = data.T

    # Create the heatmap with a suitable colormap.
    plt.figure(figsize=(12, 6))
    im = plt.imshow(data_T, aspect="auto", cmap="viridis", origin="lower")
    plt.colorbar(im, label="Activation")

    # Label the x-axis, y-axis and add a title.
    plt.xlabel("Time")
    plt.ylabel("Latent Dimension")
    plt.title("Latent Dimensions Heatmap")

    plt.tight_layout()
    plt.show()
