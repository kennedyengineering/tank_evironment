#! /bin/bash

OUTPUT_PATH="tb_package"
echo "Generating TensorBoard package : ${OUTPUT_PATH}"
mkdir "${OUTPUT_PATH}"

extract() {
    local log_dir="$1"
    local name="$2"

    local output_path="${OUTPUT_PATH}/${name}"

    echo ""
    echo "Extracting TensorBoard logs from ${log_dir}"
    echo "Saving under ${output_path}"
    echo ""

    python3 python/extract_tensorboard.py "${log_dir}" "${output_path}"
}

# Base Model - Train_v1_base                         : 20250417_053308
extract logs/tank_game_environment_v1_20250417-053308_1/ model_train_v1_base

# LSTM Model - Train_v1                              : 20250514_180254
extract logs/tank_game_environment_v1_20250514-180254_1/ model_train_v1

# LSTM Finetuned Model - Train_v0                    : 20250515_233239
extract logs/tank_game_environment_v0_20250515-233239_0/ model_finetuned_train_v0

# LSTM Finetuned Model - Train_v0 - No Dense Rewards : 20250515_234037
extract logs/tank_game_environment_v0_20250515-234037_0/ model_finetuned_train_v0_no_dense

# LSTM Finetuned Model - Train_v2                    : 20250515_234713
extract logs/tank_game_environment_v1_20250515-234713_0/ model_finetuned_train_v2

# LSTM Finetuned Model - Train_v2 - No Dense Rewards : 20250515_234926
extract logs/tank_game_environment_v1_20250515-234926_0/ model_finetuned_train_v2_no_dense
