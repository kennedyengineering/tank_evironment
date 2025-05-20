import glob
import os
import re
import ast
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np


def parse_log_text(log_text):
    """
    Returns a dict with keys:
      - rewards: list of float
      - durations: list of int
      - win_log: list of int (0 or 1)
    """

    result = {}

    # Rewards list
    rewards_match = re.search(r"Rewards:\s*(\[[^\]]+\])", log_text)
    if rewards_match:
        result["rewards"] = ast.literal_eval(rewards_match.group(1))

    # Episode durations list
    durations_match = re.search(r"Episode Durations:\s*(\[[^\]]+\])", log_text)
    if durations_match:
        result["durations"] = ast.literal_eval(durations_match.group(1))

    # Win log: capture 0/1 entries (float or int)
    win_match = re.search(r"Win Log:\s*\[([^\]]+)\]", log_text, re.DOTALL)
    if win_match:
        entries = re.findall(r"\b([01])(?:\.0*)?\b", win_match.group(1))
        result["win_log"] = [int(e) for e in entries]

    return result


if __name__ == "__main__":

    paths = glob.glob("analysis_data/comp_package/*/logs/*.txt")

    records = []

    for path in paths:
        # --- derive agent names from the directory structure ---
        agent_pair = os.path.basename(os.path.dirname(os.path.dirname(path)))
        agent1_name, _, agent2_name = agent_pair.partition("_vs_")

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
                "agent1": agent1_name,
                "agent2": agent2_name,
                "wins": wins,
                "ties": ties,
                "losses": losses,
                "episodes": episodes,
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

    for map_name in df["map"].unique():

        sub = df[df["map"] == map_name]

        # pivot to square matrix (agent1 down the rows, agent2 across the columns)
        mat = sub.pivot(index="agent1", columns="agent2", values="win_rate").reindex(
            index=agents, columns=agents, fill_value=np.nan
        )

        # draw heatmap
        fig, ax = plt.subplots(figsize=(6, 6))
        cax = ax.imshow(mat.values, vmin=0, vmax=1, aspect="equal")

        ax.set_xticks(np.arange(len(agents)))
        ax.set_yticks(np.arange(len(agents)))
        ax.set_xticklabels(agents, rotation=90)
        ax.set_yticklabels(agents)
        ax.set_xlabel("Agent 2")
        ax.set_ylabel("Agent 1")
        ax.set_title(f"Win‐rate on {map_name}")

        fig.colorbar(cax, ax=ax, label="Win Rate")
        plt.tight_layout()
        plt.show()
