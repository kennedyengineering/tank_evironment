# Tank Game (@kennedyengineering)

# adapted from https://stable-baselines3.readthedocs.io/en/master/guide/examples.html#learning-rate-schedule
# and from https://pytorch.org/docs/stable/generated/torch.optim.lr_scheduler.CosineAnnealingLR.html

from typing import Callable
import numpy as np


def cosine_schedule(
    initial_value: float, final_value: float = 5e-8
) -> Callable[[float], float]:
    """
    Cosine schedule.

    :param initial_value: initial value
    :param final_value: final value
    :return: schedule that computes
      current value depending on remaining progress
    """

    def func(progress_remaining: float) -> float:
        """
        Progress will decrease from 1 (beginning) to 0.

        :param progress_remaining:
        :return: current value
        """
        return final_value + 0.5 * (initial_value - final_value) * (
            1 + np.cos(progress_remaining * np.pi)
        )

    return func
