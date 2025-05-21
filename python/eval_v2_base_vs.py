# Tank Game (@kennedyengineering)

from tank_game_environment import tank_game_environment_v1

from tank_game_agent.feature_extactor.feature_extractor_wrapper import (
    FeatureExtractorWrapper,
)

from tank_game_agent.analysis.plot_actions import plot_actions

from tank_game_agent.vec_env.vec_env import TankVecEnv

import os
import argparse
import numpy as np

from gymnasium import spaces

from moviepy.video.io.ImageSequenceClip import ImageSequenceClip

from stable_baselines3 import PPO
from stable_baselines3.common.evaluation import evaluate_policy
from stable_baselines3.common.env_util import make_vec_env
from stable_baselines3.common.torch_layers import FlattenExtractor

from sb3_contrib import RecurrentPPO


def eval(
    base_model_path,
    lstm_model_path,
    map_name,
    record_video,
    record_actions,
    num_episodes,
    base_deterministic,
    lstm_deterministic,
):
    """
    Compare a base agent to a derived LSTM agent (where the feature extractors are identical).
    """

    if record_video is not None:
        print(f"Recording video, saving to {record_video}")

    if record_actions:
        print(f"Recording actions, saving to {record_actions}")

    # Configuration variables
    seed = 100
    device = "cpu"

    # Load LSTM agent
    print(f"Loading LSTM model from {lstm_model_path}.")
    lstm_model = RecurrentPPO.load(lstm_model_path, device=device, seed=seed)

    # Load feature extractor / base model
    print(f"Loading feature extractor model / base model from {base_model_path}")
    base_model = PPO.load(
        base_model_path,
        device=device,
        seed=seed,
    )
    feature_extractor = base_model.policy.features_extractor
    feature_extractor.eval()

    # Build the wrapper’s obs_space
    wrapped_space = spaces.Box(
        low=-np.inf,
        high=np.inf,
        shape=(feature_extractor.features_dim,),
        dtype=np.float32,
    )

    # Swap out *all* extractors on the base policy
    flattener = FlattenExtractor(wrapped_space)
    base_model.policy.features_extractor = flattener
    if hasattr(base_model.policy, "pi_features_extractor"):
        base_model.policy.pi_features_extractor = flattener
    if hasattr(base_model.policy, "vf_features_extractor"):
        base_model.policy.vf_features_extractor = flattener

    # Override so .predict() accepts the wrapper's output shape
    base_model.observation_space = wrapped_space
    base_model.policy.observation_space = wrapped_space

    # Define the environment
    vec_env_kwargs = dict(
        opponent_model=lstm_model,
        opponent_predict_deterministic=lstm_deterministic,
    )
    env_kwargs = dict(
        map_id=map_name,
        render_mode="rgb_array" if record_video else "human",
        wrappers=[FeatureExtractorWrapper],
        wrappers_kwargs={
            FeatureExtractorWrapper: dict(
                feature_extractor=feature_extractor,
            )
        },
    )

    # Create environment
    eval_env = make_vec_env(
        tank_game_environment_v1.env_fn,
        n_envs=1,
        seed=seed,
        env_kwargs=env_kwargs,
        vec_env_cls=TankVecEnv,
        vec_env_kwargs=vec_env_kwargs,
    )

    eval_env_name = eval_env.metadata["name"]

    # Log actions
    actions = []

    def log_actions(locals_, _):
        if record_actions:
            actions.append(locals_["actions"])

            done = locals_["done"]
            if done:
                # Prepare plot
                np_actions = np.array(actions)
                np_actions = np.squeeze(actions)

                # Plot and save plot to disk
                idx = locals_["episode_counts"][0]

                base, ext = os.path.splitext(record_actions)
                filename = f"{base}_{idx}.png"
                plot_actions(np_actions, save_path=filename)
                filename = f"{base}_{idx}.pdf"
                plot_actions(np_actions, save_path=filename)

                # Reset buffer
                actions.clear()

    # Log win stats
    win_log = np.zeros(num_episodes)
    tie_log = np.zeros(num_episodes)
    loss_log = np.zeros(num_episodes)
    timeout_log = np.zeros(num_episodes)

    def log_win_stats_callback(locals_, _):
        done = locals_["done"]
        if done:
            infos = locals_["infos"][0]
            idx = locals_["episode_counts"][0]

            won = "hit" in infos and not "hit_by" in infos
            win_log[idx] = int(won)

            tie = "hit" in infos and "hit_by" in infos
            tie_log[idx] = int(tie)

            lost = not "hit" in infos and "hit_by" in infos
            loss_log[idx] = int(lost)

            timeout = not "hit" in infos and not "hit_by" in infos
            timeout_log[idx] = int(timeout)

    # Log frames
    frames = []

    def log_frames_callback(locals_, _):
        if record_video:
            frames.append(locals_["env"].render())

            done = locals_["done"]
            if done:
                # Save video to disk
                idx = locals_["episode_counts"][0]

                base, ext = os.path.splitext(record_video)
                filename = f"{base}_{idx}{ext}"

                clip = ImageSequenceClip(frames, fps=30)
                clip.write_videofile(
                    filename,
                    audio=False,
                    logger=None,
                )

                # Reset frame buffer
                frames.clear()

    # Create callback list
    callbacks = [log_actions, log_win_stats_callback, log_frames_callback]
    master_callback = lambda locals_, _: [fn(locals_, _) for fn in callbacks]

    # Run evaluation
    print(f"Starting evaluation on {eval_env_name}. (num_episodes={num_episodes})")
    rewards = evaluate_policy(
        base_model,
        eval_env,
        n_eval_episodes=num_episodes,
        deterministic=base_deterministic,
        render=False,
        return_episode_rewards=True,
        callback=master_callback,
    )
    print("Rewards: ", rewards[0])
    print("Episode Durations: ", rewards[1])
    print("Average Reward: ", np.mean(rewards[0]))
    print("Average Duration: ", np.mean(rewards[1]))
    print("Win Log:\t", win_log)
    print("Tie Log:\t", tie_log)
    print("Loss Log:\t", loss_log)
    print("Timeout Log:\t", timeout_log)
    print("Win Rate: ", np.count_nonzero(win_log) / win_log.size)
    print("Tie Rate: ", np.count_nonzero(tie_log) / tie_log.size)
    print("Loss Rate: ", np.count_nonzero(loss_log) / loss_log.size)
    print("Timeout Rate: ", np.count_nonzero(timeout_log) / timeout_log.size)


if __name__ == "__main__":

    parser = argparse.ArgumentParser(description="Train or evaluate a model.")

    # Create subparsers for 'train' and 'eval' modes
    subparsers = parser.add_subparsers(dest="mode", required=True)

    # Evaluation mode
    eval_parser = subparsers.add_parser("eval", help="Run model evaluation.")
    eval_parser.add_argument(
        "base_model_path", type=str, help="Path to a trained base model."
    )
    eval_parser.add_argument(
        "lstm_model_path", type=str, help="Path to a trained LSTM model."
    )
    eval_parser.add_argument(
        "--map", type=str, default="Random", help="Name of the map."
    )
    eval_parser.add_argument(
        "--record-video",
        type=str,
        default=None,
        help="Path to output video file (e.g. output.mp4).",
    )
    eval_parser.add_argument(
        "--record-actions",
        type=str,
        default=None,
        help="Path to output plot files (e.g. actions.png or actions.pdf).",
    )
    eval_parser.add_argument(
        "--episodes", type=int, default=20, help="Number of episodes to run."
    )
    eval_parser.add_argument(
        "--stochastic",
        action="store_true",
        help="Perform stochastic inference of the base agent or not.",
    )
    eval_parser.add_argument(
        "--opponent-stochastic",
        action="store_true",
        help="Perform stochastic inference of the LSTM agent or not.",
    )

    # Parse arguments
    args = parser.parse_args()

    # Run evaluation
    print("Evaluation mode selected.")
    eval(
        args.base_model_path,
        args.lstm_model_path,
        args.map,
        args.record_video,
        args.record_actions,
        args.episodes,
        not args.stochastic,
        not args.opponent_stochastic,
    )
