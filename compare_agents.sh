#! /bin/bash

OUTPUT_PATH="comp_package"
echo "Generating comparison package : ${OUTPUT_PATH}"
mkdir "${OUTPUT_PATH}"

comp() {
    local weights="$1"
    local opponent_weights="$2"
    local name="$3"
    local output_path="${OUTPUT_PATH}/${name}"
    local log_output_path="${output_path}/logs"
    local video_output_path="${output_path}/videos"

    echo ""
    echo "Running comprehensive comparison between ${weights} and ${opponent_weights}"
    echo "Saving under ${output_path}"
    echo ""

    mkdir "${output_path}"
    mkdir "${log_output_path}"
    mkdir "${video_output_path}"

    run_deterministic() {
        local run_name="$1"
        local episodes="$2"
        local map="$3"

        mkdir "${video_output_path}/${run_name}"
        python3 python/train_v2.py eval "${weights}" "${opponent_weights}" --episodes "${episodes}" --map "${map}" --record-video "${video_output_path}/${run_name}/${run_name}.mp4" > "${log_output_path}/${run_name}.txt"
    }

    run_stochastic() {
        local run_name="$1"
        local episodes="$2"
        local map="$3"

        mkdir "${video_output_path}/${run_name}"
        python3 python/train_v2.py eval "${weights}" "${opponent_weights}" --episodes "${episodes}" --map "${map}" --record-video "${video_output_path}/${run_name}/${run_name}.mp4" --stochastic --stochastic-opponent > "${log_output_path}/${run_name}.txt"
    }

    ### Deterministic ###

    # Deterministic Policy : Map Random, 100 Episodes
    run_deterministic deterministic_random_100 100 Random

    # Deterministic Policy : Map Boulder, 1 Episode
    run_deterministic deterministic_boulder_1 1 Boulder

    # Deterministic Policy : Map GridAligned, 1 Episode
    run_deterministic deterministic_grid_aligned_1 1 GridAligned

    # Deterministic Policy : Map GridStaggered, 1 Episode
    run_deterministic deterministic_grid_staggered_1 1 GridStaggered

    # Deterministic Policy : Map Quincunx, 1 Episode
    run_deterministic deterministic_quincunx_1 1 Quincunx

    # Deterministic Policy : Map WallLarge, 1 Episode
    run_deterministic deterministic_wall_large_1 1 WallLarge

    # Deterministic Policy : Map WallMedium, 1 Episode
    run_deterministic deterministic_wall_medium_1 1 WallMedium

    # Deterministic Policy : Map WallSmall, 1 Episode
    run_deterministic deterministic_wall_small_1 1 WallSmall

    ### Stochastic ###

    # Stochastic Policy : Map Random, 100 Episodes
    run_stochastic stochastic_random_100 100 Random

    # Stochastic Policy : Map Boulder, 100 Episodes
    run_stochastic stochastic_boulder_100 100 Boulder

    # Stochastic Policy : Map GridAligned, 100 Episodes
    run_stochastic stochastic_grid_aligned_100 100 GridAligned

    # Stochastic Policy : Map GridStaggered, 100 Episodes
    run_stochastic stochastic_grid_staggered_100 100 GridStaggered

    # Stochastic Policy : Map Quincunx, 100 Episodes
    run_stochastic stochastic_quincunx_100 100 Quincunx

    # Stochastic Policy : Map WallLarge, 100 Episodes
    run_stochastic stochastic_wall_large_100 100 WallLarge

    # Stochastic Policy : Map WallMedium, 100 Episodes
    run_stochastic stochastic_wall_medium_100 100 WallMedium

    # Stochastic Policy : Map WallSmall, 100 Episodes
    run_stochastic stochastic_wall_small_100 100 WallSmall
}

# LSTM Model - Train_v1                              : 20250514_180254
model_train_v1_weights=weights/tank_game_environment_v1_20250514-180254/tank_game_environment_v1_20250514-180254.zip

# LSTM Finetuned Model - Train_v0                    : 20250515_233239
model_finetuned_train_v0_weights=weights/tank_game_environment_v0_20250515-233239/tank_game_environment_v0_20250515-233239.zip

# LSTM Finetuned Model - Train_v0 - No Dense Rewards : 20250515_234037
model_finetuned_train_v0_no_dense_weights=weights/tank_game_environment_v0_20250515-234037/tank_game_environment_v0_20250515-234037.zip

# LSTM Finetuned Model - Train_v2                    : 20250515_234713
model_finetuned_train_v2_weights=weights/tank_game_environment_v1_20250515-234713/tank_game_environment_v1_20250515-234713.zip

# LSTM Finetuned Model - Train_v2 - No Dense Rewards : 20250515_234926
model_finetuned_train_v2_no_dense_weights=weights/tank_game_environment_v1_20250515-234926/tank_game_environment_v1_20250515-234926.zip

# LSTM Model vs *
comp "${model_train_v1_weights}" "${model_finetuned_train_v0_weights}" model_train_v1_vs_model_finetuned_train_v0
comp "${model_train_v1_weights}" "${model_finetuned_train_v0_no_dense_weights}" model_train_v1_vs_model_finetuned_train_v0_no_dense
comp "${model_train_v1_weights}" "${model_finetuned_train_v2_weights}" model_train_v1_vs_model_finetuned_train_v2
comp "${model_train_v1_weights}" "${model_finetuned_train_v2_no_dense_weights}" model_train_v1_vs_model_finetuned_train_v2_no_dense

# LSTM Finetuned Model - Train_v0 vs **
comp "${model_finetuned_train_v0_weights}" "${model_finetuned_train_v0_no_dense_weights}" model_finetuned_train_v0_vs_model_finetuned_train_v0_no_dense
comp "${model_finetuned_train_v0_weights}" "${model_finetuned_train_v2_weights}" model_finetuned_train_v0_vs_model_finetuned_train_v2
comp "${model_finetuned_train_v0_weights}" "${model_finetuned_train_v2_no_dense_weights}" model_finetuned_train_v0_vs_model_finetuned_train_v2_no_dense

# LSTM Finetuned Model - Train_v0 - No Dense Rewards vs ***
comp "${model_finetuned_train_v0_no_dense_weights}" "${model_finetuned_train_v2_weights}" model_finetuned_train_v0_no_dense_vs_model_finetuned_train_v2
comp "${model_finetuned_train_v0_no_dense_weights}" "${model_finetuned_train_v2_no_dense_weights}" model_finetuned_train_v0_no_dense_vs_model_finetuned_train_v2_no_dense

# LSTM Finetuned Model - Train_v2 vs ****
comp "${model_finetuned_train_v2_weights}" "${model_finetuned_train_v2_no_dense_weights}" model_finetuned_train_v2_vs_model_finetuned_train_v2_no_dense
