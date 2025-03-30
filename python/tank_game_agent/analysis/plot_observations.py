import numpy as np
import matplotlib.pyplot as plt
from numpy.typing import NDArray


def plot_observations(observations: NDArray[np.float64]) -> None:
    """
    Plots a lidar heatmap and velocity measurements from a given observation array.

    Parameters:
        observations (NDArray[np.float64]): A 2D NumPy array where each row represents
                                            an observation at a given time step.
                                            - Columns 0 to 359: Lidar distance measurements (in degrees)
                                            - Column 360: Local velocity X (m/s)
                                            - Column 361: Local velocity Y (m/s)
                                            - Column 362: Local angular velocity (rad/s)
                                            - Column 363: Reload counter

    Returns:
        None: This function displays plots but does not return any value.
    """
    # Set global figure styling
    plt.style.use("seaborn-v0_8-darkgrid")  # Professional-looking style
    plt.rcParams.update(
        {
            "font.size": 14,  # Increase font size for readability
            "axes.labelsize": 14,  # Label font size
            "axes.titlesize": 16,  # Title font size
            "xtick.labelsize": 12,  # X-axis tick font size
            "ytick.labelsize": 12,  # Y-axis tick font size
        }
    )

    ### --- Lidar Heatmap --- ###
    lidar = observations[:, :360].T  # Transpose to align degrees vertically

    fig, ax = plt.subplots(figsize=(10, 6))  # Set figure size
    img = ax.imshow(lidar, cmap="viridis", origin="lower", aspect="auto")
    cbar = plt.colorbar(img, ax=ax, label="Distance")

    ax.set_title("Lidar Distance Measurements")
    ax.set_xlabel("Time")
    ax.set_ylabel("Degree")

    plt.show()

    ### --- Velocity Plots (All in One Figure) --- ###
    fig, axes = plt.subplots(4, 1, figsize=(12, 10), sharex=True)

    # Local Velocity X
    vel_x = observations[:, 360]
    axes[0].plot(vel_x, color="blue", linewidth=2)
    axes[0].set_title("Local Velocity X")
    axes[0].set_ylabel("Velocity (m/s)")
    axes[0].grid(True)

    # Local Velocity Y
    vel_y = observations[:, 361]
    axes[1].plot(vel_y, color="green", linewidth=2)
    axes[1].set_title("Local Velocity Y")
    axes[1].set_ylabel("Velocity (m/s)")
    axes[1].grid(True)

    # Local Angular Velocity
    vel_ang = observations[:, 362]
    axes[2].plot(vel_ang, color="red", linewidth=2)
    axes[2].set_title("Local Angular Velocity")
    axes[2].set_ylabel("Velocity (rad/s)")
    axes[2].grid(True)

    # Reload Counter
    counter = observations[:, 363]
    axes[3].plot(counter, color="purple", linewidth=2)
    axes[3].set_title("Reload Counter")
    axes[3].set_xlabel("Time")
    axes[3].set_ylabel("Counter Value")
    axes[3].grid(True)

    plt.tight_layout()  # Adjust layout for better spacing
    plt.show()
