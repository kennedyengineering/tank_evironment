#! /bin/bash

OUTPUT_PATH="comp_package"
echo "Generating comparison package : ${OUTPUT_PATH}"
mkdir "${OUTPUT_PATH}"

comp() {
    local weights="$1"
    local opponent_weights="$2"
    local name="$3"
    local is_base="${4:-false}"

    local output_path="${OUTPUT_PATH}/${name}"
    local log_output_path="${output_path}/logs"
    local video_output_path="${output_path}/videos"
    local action_output_path="${output_path}/actions"

    local python_script="python/train_v2.py"
    if [[ "$is_base" == "true" ]]; then
        python_script="python/eval_v2_base_vs.py"
    fi

    echo ""
    echo "Running comprehensive comparison between ${weights} and ${opponent_weights}"
    echo "Saving under ${output_path}"
    echo ""

    mkdir "${output_path}"
    mkdir "${log_output_path}"
    mkdir "${video_output_path}"
    mkdir "${action_output_path}"

    run_deterministic() {
        local run_name="$1"
        local episodes="$2"
        local map="$3"

        mkdir "${video_output_path}/${run_name}"
        mkdir "${action_output_path}/${run_name}"
        python3 "${python_script}" eval "${weights}" "${opponent_weights}" --episodes "${episodes}" --map "${map}" --record-video "${video_output_path}/${run_name}/${run_name}.mp4" --record-actions "${action_output_path}/${run_name}/${run_name}.unused" > "${log_output_path}/${run_name}.txt"
    }

    run_stochastic() {
        local run_name="$1"
        local episodes="$2"
        local map="$3"

        mkdir "${video_output_path}/${run_name}"
        mkdir "${action_output_path}/${run_name}"
        python3 "${python_script}" eval "${weights}" "${opponent_weights}" --episodes "${episodes}" --map "${map}" --record-video "${video_output_path}/${run_name}/${run_name}.mp4" --record-actions "${action_output_path}/${run_name}/${run_name}.unused" --stochastic --opponent-stochastic > "${log_output_path}/${run_name}.txt"
    }

    ### Deterministic ###

    # Deterministic Policy : Map Random, 400 Episodes
    run_deterministic deterministic_random_400 400 Random

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

    # Stochastic Policy : Map Random, 400 Episodes
    run_stochastic stochastic_random_400 400 Random

    # Stochastic Policy : Map Boulder, 400 Episodes
    run_stochastic stochastic_boulder_400 400 Boulder

    # Stochastic Policy : Map GridAligned, 400 Episodes
    run_stochastic stochastic_grid_aligned_400 400 GridAligned

    # Stochastic Policy : Map GridStaggered, 400 Episodes
    run_stochastic stochastic_grid_staggered_400 400 GridStaggered

    # Stochastic Policy : Map Quincunx, 400 Episodes
    run_stochastic stochastic_quincunx_400 400 Quincunx

    # Stochastic Policy : Map WallLarge, 400 Episodes
    run_stochastic stochastic_wall_large_400 400 WallLarge

    # Stochastic Policy : Map WallMedium, 400 Episodes
    run_stochastic stochastic_wall_medium_400 400 WallMedium

    # Stochastic Policy : Map WallSmall, 400 Episodes
    run_stochastic stochastic_wall_small_400 400 WallSmall
}

# Base Model - Train_v1_base                         : 20250417_053308
model_train_v1_base_weights=weights/tank_game_environment_v1_20250417-053308/tank_game_environment_v1_20250417-053308.zip

# LSTM Model - Train_v1                              : 20250514_180254
model_train_v1_weights=weights/tank_game_environment_v1_20250514-180254/tank_game_environment_v1_20250514-180254.zip

# LSTM Finetuned Model - Train_v0                    : 20250515_233239
model_finetuned_train_v0_weights=weights/tank_game_environment_v0_20250515-233239/tank_game_environment_v0_20250515-233239.zip

# LSTM Finetuned Model - Train_v0 - No Dense Rewards : 20250515_234037
model_finetuned_train_v0_no_dense_weights=weights/tank_game_environment_v0_20250515-234037/tank_game_environment_v0_20250515-234037.zip

# LSTM Finetuned Model - Train_v2                    : 20250521_013453
model_finetuned_train_v2_weights=weights/tank_game_environment_v1_20250521-013453/tank_game_environment_v1_20250521-013453.zip

# LSTM Finetuned Model - Train_v2 - No Dense Rewards : 20250521_013649
model_finetuned_train_v2_no_dense_weights=weights/tank_game_environment_v1_20250521-013649/tank_game_environment_v1_20250521-013649.zip

# Base Model vs *
comp "${model_train_v1_base_weights}" "${model_train_v1_weights}" model_train_v1_base_vs_model_train_v1 true &
comp "${model_train_v1_base_weights}" "${model_finetuned_train_v0_weights}" model_train_v1_base_vs_model_finetuned_train_v0 true &
comp "${model_train_v1_base_weights}" "${model_finetuned_train_v0_no_dense_weights}" model_train_v1_base_vs_model_finetuned_train_v0_no_dense true &
comp "${model_train_v1_base_weights}" "${model_finetuned_train_v2_weights}" model_train_v1_base_vs_model_finetuned_train_v2 true &
comp "${model_train_v1_base_weights}" "${model_finetuned_train_v2_no_dense_weights}" model_train_v1_base_vs_model_finetuned_train_v2_no_dense true &

# LSTM Model vs **
comp "${model_train_v1_weights}" "${model_finetuned_train_v0_weights}" model_train_v1_vs_model_finetuned_train_v0 &
comp "${model_train_v1_weights}" "${model_finetuned_train_v0_no_dense_weights}" model_train_v1_vs_model_finetuned_train_v0_no_dense &
comp "${model_train_v1_weights}" "${model_finetuned_train_v2_weights}" model_train_v1_vs_model_finetuned_train_v2 &
comp "${model_train_v1_weights}" "${model_finetuned_train_v2_no_dense_weights}" model_train_v1_vs_model_finetuned_train_v2_no_dense &

# LSTM Finetuned Model - Train_v0 vs ***
comp "${model_finetuned_train_v0_weights}" "${model_finetuned_train_v0_no_dense_weights}" model_finetuned_train_v0_vs_model_finetuned_train_v0_no_dense &
comp "${model_finetuned_train_v0_weights}" "${model_finetuned_train_v2_weights}" model_finetuned_train_v0_vs_model_finetuned_train_v2 &
comp "${model_finetuned_train_v0_weights}" "${model_finetuned_train_v2_no_dense_weights}" model_finetuned_train_v0_vs_model_finetuned_train_v2_no_dense &

# LSTM Finetuned Model - Train_v0 - No Dense Rewards vs ****
comp "${model_finetuned_train_v0_no_dense_weights}" "${model_finetuned_train_v2_weights}" model_finetuned_train_v0_no_dense_vs_model_finetuned_train_v2 &
comp "${model_finetuned_train_v0_no_dense_weights}" "${model_finetuned_train_v2_no_dense_weights}" model_finetuned_train_v0_no_dense_vs_model_finetuned_train_v2_no_dense &

# LSTM Finetuned Model - Train_v2 vs *****
comp "${model_finetuned_train_v2_weights}" "${model_finetuned_train_v2_no_dense_weights}" model_finetuned_train_v2_vs_model_finetuned_train_v2_no_dense &

wait
