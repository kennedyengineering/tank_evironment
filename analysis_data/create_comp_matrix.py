import glob
import os
import re
import ast
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

"""
Read as : agent1 vs agent2, agent1 wins % of the time against agent2.
"""


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

    output_dir = "analysis_data/comp_matrix/"
    os.makedirs(output_dir, exist_ok=True)

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

        # pivot out losses, ties and episodes in the same layout
        loss_mat = sub.pivot(index="agent1", columns="agent2", values="losses").reindex(
            index=agents, columns=agents
        )
        ties_mat = sub.pivot(index="agent1", columns="agent2", values="ties").reindex(
            index=agents, columns=agents
        )
        ep_mat = sub.pivot(index="agent1", columns="agent2", values="episodes").reindex(
            index=agents, columns=agents
        )

        # compute the "reverse" win-rate for the lower half as losses / episodes
        reverse = loss_mat.T / ep_mat.T

        # fill only the strictly lower triangle of mat with reverse
        n = len(agents)
        for i in range(n):
            for j in range(n):
                if i > j and np.isnan(mat.iat[i, j]):
                    mat.iat[i, j] = reverse.iat[i, j]

        # draw heatmap
        fig, ax = plt.subplots(figsize=(6, 6))
        cax = ax.imshow(mat.values, vmin=0, vmax=1, aspect="equal")

        # annotate each cell with the win rate
        n = len(agents)
        for i in range(n):
            for j in range(n):
                rate = mat.iat[i, j]
                if not np.isnan(rate):
                    # choose a contrasting text color
                    color = "white" if rate < 0.5 else "black"
                    ax.text(
                        j,
                        i,
                        str(rate),
                        ha="center",
                        va="center",
                        color=color,
                        fontsize=7,
                    )

        ax.set_xticks(np.arange(len(agents)))
        ax.set_yticks(np.arange(len(agents)))
        ax.set_xticklabels(agents, rotation=45, ha="right", fontsize=8)
        ax.set_yticklabels(agents, fontsize=8)
        ax.set_xlabel("Opponent")
        ax.set_ylabel("Agent")
        ax.set_title(f"Win-rate on {map_name}")

        fig.colorbar(
            cax,
            ax=ax,
            orientation="vertical",
            fraction=0.04,
            pad=0.02,
            label="Win Rate",
        )
        plt.tight_layout(rect=[0, 0, 0.9, 1])
        plt.savefig(os.path.join(output_dir, f"comp_matrix_{map_name}.png"))
        plt.savefig(os.path.join(output_dir, f"comp_matrix_{map_name}.pdf"))
