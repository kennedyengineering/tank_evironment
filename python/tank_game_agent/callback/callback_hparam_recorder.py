# Tank Game (@kennedyengineering)

# adapted from https://stable-baselines3.readthedocs.io/en/master/guide/tensorboard.html#logging-hyperparameters

from stable_baselines3.common.callbacks import BaseCallback
from stable_baselines3.common.logger import HParam


class HParamRecorderCallback(BaseCallback):
    def __init__(self, hparam_dict: dict = {}, metric_dict: dict = {}):
        """
        Saves the hyperparameters and metrics at the start of the training, and logs them to TensorBoard.

        :param hparam_dict: Parameters to be logged
        :param metric_dict: Metrics to be shown with parameters
        """

        super().__init__()
        self._hparam_dict = hparam_dict
        self._metric_dict = metric_dict

    def _on_training_start(self) -> None:
        hparam_dict = {
            "algorithm": self.model.__class__.__name__,
        } | self._hparam_dict

        # define the metrics that will appear in the `HPARAMS` Tensorboard tab by referencing their tag
        # Tensorbaord will find & display metrics from the `SCALARS` tab
        metric_dict = {
            "rollout/ep_len_mean": 0,
            "rollout/ep_rew_mean": 0,
        } | self._metric_dict

        self.logger.record(
            "hparams",
            HParam(hparam_dict, metric_dict),
            exclude=("stdout", "log", "json", "csv"),
        )

    def _on_step(self) -> bool:
        return True
