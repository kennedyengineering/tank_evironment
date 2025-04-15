# Tank Game (@kennedyengineering)

# adapted from https://stable-baselines3.readthedocs.io/en/master/guide/examples.html#learning-rate-schedule
# and from https://keras.io/api/optimizers/learning_rate_schedules/exponential_decay/

from typing import Callable


# TODO: decay to an optional final value which is less than initial value
def exponential_schedule(
    initial_value: float, final_value: float = 5e-8
) -> Callable[[float], float]:
    """
    Exponential schedule.

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
        return initial_value * (final_value / initial_value) ** (1 - progress_remaining)

    return func
