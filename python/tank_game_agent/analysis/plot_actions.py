import numpy as np
import matplotlib.pyplot as plt
from numpy.typing import NDArray


def plot_actions(actions: NDArray[np.float64]) -> None:
    """
    Plots the actions taken over time.

    Parameters:
        actions (NDArray[np.float64]): A 2D NumPy array where each row represents
                                       an action at a given time step.
                                       - Column 0: Left tread speed
                                       - Column 1: Right tread speed
                                       - Column 2: Fire gun (binary: 0 or 1)

    Returns:
        None: This function displays plots but does not return any value.
    """

    ### --- Actions Plot --- ###
    fig, axes = plt.subplots(3, 1, figsize=(12, 8), sharex=True)

    # Left Tread Speed
    left_tread = actions[:, 0]
    axes[0].plot(left_tread, color="blue", linewidth=2)
    axes[0].set_title("Left Tread Speed")
    axes[0].set_ylabel("Speed")
    axes[0].grid(True)

    # Right Tread Speed
    right_tread = actions[:, 1]
    axes[1].plot(right_tread, color="green", linewidth=2)
    axes[1].set_title("Right Tread Speed")
    axes[1].set_ylabel("Speed")
    axes[1].grid(True)

    # Fire Gun
    fire_gun = actions[:, 2]
    axes[2].plot(fire_gun, color="red", linewidth=2, linestyle="dashed", marker="o")
    axes[2].set_title("Fire Gun Action")
    axes[2].set_xlabel("Time")
    axes[2].set_ylabel("Fire (0 or 1)")
    axes[2].grid(True)

    plt.tight_layout()  # Adjust layout for better spacing
    plt.show()
