import glob
import os
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

from create_comp_matrix import parse_log_text


if __name__ == "__main__":

    paths = glob.glob("analysis_data/eval_package/*/logs/*.txt")

    records = []

    for path in paths:
        # --- derive agent names from the directory structure ---
        agent_name = os.path.basename(os.path.dirname(os.path.dirname(path)))

        # --- derive map name and episode count from the filename ---
        map_episode_pair = os.path.splitext(os.path.basename(path))[0]
        map_name, episodes_str = map_episode_pair.rsplit("_", 1)
        episodes = int(episodes_str)

        # --- read & parse the log ---
        with open(path, "r") as f:
            text = f.read()
        info = parse_log_text(text)

        # --- validate naming convention ---
        assert episodes == len(info["win_log"])
        assert episodes == len(info["durations"])
        assert episodes == len(info["rewards"])

        # --- compute statistics ---
        wins = info["win_log"].count(1)
        ties = info["durations"].count(1002)
        losses = episodes - wins - ties

        # --- add to record ---
        records.append(
            {
                "map": map_name,
                "agent": agent_name,
                "wins": wins,
                "ties": ties,
                "losses": losses,
                "episodes": episodes,
                "average_duration": np.mean(info["durations"]),
            }
        )

    df = pd.DataFrame(records)

    df["win_rate"] = df["wins"] / df["episodes"]

    # agents = sorted(
    #     set(df['agent1']).union(df['agent2'])
    # )

    agents = [
        "model_train_v1_base",
        "model_train_v1",
        "model_finetuned_train_v0",
        "model_finetuned_train_v0_no_dense",
        "model_finetuned_train_v2",
        "model_finetuned_train_v2_no_dense",
    ]

    maps = sorted(df["map"].unique())

    deterministic_maps = [m for m in maps if m.startswith("deterministic_")]
    stochastic_maps = [m for m in maps if m.startswith("stochastic_")]

    # --- Plot Win-Rate ---
    fig, axes = plt.subplots(2, 1, figsize=(6, 7), sharex=False)
    fig.suptitle("Agent Win-Rates Across Maps")

    for ax, subset_maps, title in zip(
        axes,
        [deterministic_maps, stochastic_maps],
        ["Deterministic Policy", "Stochastic Policy"],
    ):
        mat = df.pivot(index="agent", columns="map", values="win_rate")
        mat = mat.reindex(index=agents, columns=subset_maps)

        cax = ax.imshow(mat.values, vmin=0, vmax=1, aspect="equal")
        ax.set_xticks(np.arange(len(subset_maps)))
        ax.set_yticks(np.arange(len(agents)))
        ax.set_xticklabels(subset_maps, rotation=45, ha="right", fontsize=8)
        ax.set_yticklabels(agents, fontsize=8)
        ax.set_ylabel("Agent")
        ax.set_title(title)

    # Common X label on bottom subplot
    axes[-1].set_xlabel("Map")

    # Shared colorbar
    fig.colorbar(
        cax, ax=axes, orientation="vertical", fraction=0.02, pad=0.02, label="Win-Rate"
    )
    plt.tight_layout(rect=[0, 0, 0.9, 1])
    plt.show()

    # --- Plot Duration ---
    fig, axes = plt.subplots(2, 1, figsize=(6, 7), sharex=False)
    fig.suptitle("Agent Average Steps to Completion Across Maps")

    for ax, subset_maps, title in zip(
        axes,
        [deterministic_maps, stochastic_maps],
        ["Deterministic Policy", "Stochastic Policy"],
    ):
        mat = df.pivot(index="agent", columns="map", values="average_duration")
        mat = mat.reindex(index=agents, columns=subset_maps)

        cax = ax.imshow(mat.values, vmin=0, vmax=1002, aspect="equal")
        ax.set_xticks(np.arange(len(subset_maps)))
        ax.set_yticks(np.arange(len(agents)))
        ax.set_xticklabels(subset_maps, rotation=45, ha="right", fontsize=8)
        ax.set_yticklabels(agents, fontsize=8)
        ax.set_ylabel("Agent")
        ax.set_title(title)

    # Common X label on bottom subplot
    axes[-1].set_xlabel("Map")

    # Shared colorbar
    fig.colorbar(
        cax, ax=axes, orientation="vertical", fraction=0.02, pad=0.02, label="Steps"
    )
    plt.tight_layout(rect=[0, 0, 0.9, 1])
    plt.show()
