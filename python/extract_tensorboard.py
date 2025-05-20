from tbparse import SummaryReader
import pandas as pd
import matplotlib.pyplot as plt
import argparse
import os


def save_hparam_csv(reader, output_path):

    # Make sure output directory exists
    os.makedirs(output_path, exist_ok=True)

    # Build filepath
    filename = "hparams.csv"
    filepath = os.path.join(output_path, filename)

    # Save hyperparameters to CSV
    reader.hparams.to_csv(filepath, sep="\t", index=False)


def save_hparam_pkl(reader, output_path):

    # Make sure output directory exists
    os.makedirs(output_path, exist_ok=True)

    # Build filepath
    filename = "hparams.pkl"
    filepath = os.path.join(output_path, filename)

    # Save hyperparameters to PKL
    reader.hparams.to_pickle(filepath)


def save_scalar_csv(reader, output_path):

    # Make sure output directory exists
    os.makedirs(output_path, exist_ok=True)

    # Save scalars to CSV
    df = reader.scalars
    for tag in df["tag"].unique():
        sub = df[df["tag"] == tag]

        # Build filepath
        safe_tag = tag.replace("/", "_").replace(" ", "_")
        filename = f"{safe_tag}.csv"
        filepath = os.path.join(output_path, filename)

        # Save to CSV
        sub.to_csv(filepath, sep="\t", index=False)


def save_scalar_pkl(reader, output_path):

    # Make sure output directory exists
    os.makedirs(output_path, exist_ok=True)

    # Save scalars to PKL
    df = reader.scalars
    for tag in df["tag"].unique():
        sub = df[df["tag"] == tag]

        # Build filepath
        safe_tag = tag.replace("/", "_").replace(" ", "_")
        filename = f"{safe_tag}.pkl"
        filepath = os.path.join(output_path, filename)

        # Save to PKL
        sub.to_pickle(filepath)


def save_scalar_png(reader, output_path):

    # Make sure output directory exists
    os.makedirs(output_path, exist_ok=True)

    # Save scalars to plots
    df = reader.scalars
    for tag in df["tag"].unique():
        sub = df[df["tag"] == tag]

        # Create figure
        plt.figure()
        plt.plot(sub["step"], sub["value"])
        plt.title(tag)
        plt.xlabel("Step")
        plt.ylabel("Value")
        plt.grid(True)  # optional, but helpful

        # Build filepath
        safe_tag = tag.replace("/", "_").replace(" ", "_")
        filename = f"{safe_tag}.png"
        filepath = os.path.join(output_path, filename)

        # Save plot
        plt.savefig(filepath)


def save_scalar_pdf(reader, output_path):

    # Make sure output directory exists
    os.makedirs(output_path, exist_ok=True)

    # Save scalars to plots
    df = reader.scalars
    for tag in df["tag"].unique():
        sub = df[df["tag"] == tag]

        # Create figure
        plt.figure()
        plt.plot(sub["step"], sub["value"])
        plt.title(tag)
        plt.xlabel("Step")
        plt.ylabel("Value")
        plt.grid(True)  # optional, but helpful

        # Build filepath
        safe_tag = tag.replace("/", "_").replace(" ", "_")
        filename = f"{safe_tag}.pdf"
        filepath = os.path.join(output_path, filename)

        # Save plot
        plt.savefig(filepath)


if __name__ == "__main__":
    parser = parser = argparse.ArgumentParser(
        description="Extract and save TensorBoard logged data."
    )
    parser.add_argument("log_dir", type=str, help="TensorBoard log directory.")
    parser.add_argument("out_dir", type=str, help="Directory to contain output data.")
    args = parser.parse_args()

    # read log
    reader = SummaryReader(args.log_dir)

    # export date
    save_hparam_csv(reader, args.out_dir)
    save_hparam_pkl(reader, args.out_dir)
    save_scalar_csv(reader, os.path.join(args.out_dir, "scalar_csv"))
    save_scalar_pkl(reader, os.path.join(args.out_dir, "scalar_pkl"))
    save_scalar_png(reader, os.path.join(args.out_dir, "scalar_png"))
    save_scalar_pdf(reader, os.path.join(args.out_dir, "scalar_pdf"))
