# Tank Game (@kennedyengineering)

# adapted from https://stable-baselines3.readthedocs.io/en/master/guide/examples.html#learning-rate-schedule

from typing import Callable


# TODO: decay to an optional final value which is less than initial value
def linear_schedule(initial_value: float) -> Callable[[float], float]:
    """
    Linear schedule.

    :param initial_value: initial value
    :return: schedule that computes
      current value depending on remaining progress
    """

    def func(progress_remaining: float) -> float:
        """
        Progress will decrease from 1 (beginning) to 0.

        :param progress_remaining:
        :return: current value
        """
        return progress_remaining * initial_value

    return func
