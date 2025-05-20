#! /bin/bash

OUTPUT_PATH="eval_package"
echo "Generating evaluation package : ${OUTPUT_PATH}"
mkdir "${OUTPUT_PATH}"

eval() {
    local weights="$1"
    local name="$2"
    local is_base="${3:-false}"

    local output_path="${OUTPUT_PATH}/${name}"
    local log_output_path="${output_path}/logs"
    local video_output_path="${output_path}/videos"

    local python_script="python/train_v1.py"
    if [[ "$is_base" == "true" ]]; then
        python_script="python/train_v1_base.py"
    fi

    echo ""
    echo "Running comprehensive evaluation on ${weights}"
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
        python3 "${python_script}" eval "${weights}" --episodes "${episodes}" --map "${map}" --record-video "${video_output_path}/${run_name}/${run_name}.mp4" > "${log_output_path}/${run_name}.txt"
    }

    run_stochastic() {
        local run_name="$1"
        local episodes="$2"
        local map="$3"

        mkdir "${video_output_path}/${run_name}"
        python3 "${python_script}" eval "${weights}" --episodes "${episodes}" --map "${map}" --record-video "${video_output_path}/${run_name}/${run_name}.mp4" --stochastic > "${log_output_path}/${run_name}.txt"
    }

    ### Deterministic ###

    # Deterministic Policy : Map RunFarRTL, 1 Episode
    run_deterministic deterministic_run_far_rtl_1 1 RunFarRTL

    # Deterministic Policy : Map RunMediumRTL, 1 Episode
    run_deterministic deterministic_run_medium_rtl_1 1 RunMediumRTL

    # Deterministic Policy : Map RunCloseRTL, 1 Episode
    run_deterministic deterministic_run_close_rtl_1 1 RunCloseRTL

    # Deterministic Policy : Map RunFarLTR, 1 Episode
    run_deterministic deterministic_run_far_ltr_1 1 RunFarLTR

    # Deterministic Policy : Map RunMediumLTR, 1 Episode
    run_deterministic deterministic_run_medium_ltr_1 1 RunMediumLTR

    # Deterministic Policy : Map RunCloseLTR, 1 Episode
    run_deterministic deterministic_run_close_ltr_1 1 RunCloseLTR

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

    # Stochastic Policy : Map RunFarRTL, 100 Episodes
    run_stochastic stochastic_run_far_rtl_100 100 RunFarRTL

    # Stochastic Policy : Map RunMediumRTL, 100 Episodes
    run_stochastic stochastic_run_medium_rtl_100 100 RunMediumRTL

    # Stochastic Policy : Map RunCloseRTL, 100 Episodes
    run_stochastic stochastic_run_close_rtl_100 100 RunCloseRTL

    # Stochastic Policy : Map RunFarLTR, 100 Episodes
    run_stochastic stochastic_run_far_ltr_100 100 RunFarLTR

    # Stochastic Policy : Map RunMediumLTR, 100 Episodes
    run_stochastic stochastic_run_medium_ltr_100 100 RunMediumLTR

    # Stochastic Policy : Map RunCloseLTR, 100 Episodes
    run_stochastic stochastic_run_close_ltr_100 100 RunCloseLTR

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
# Base Model - Train_v1_base                         : 20250417_053308
eval weights/tank_game_environment_v1_20250417-053308/tank_game_environment_v1_20250417-053308.zip model_train_v1_base true

# LSTM Model - Train_v1                              : 20250514_180254
eval weights/tank_game_environment_v1_20250514-180254/tank_game_environment_v1_20250514-180254.zip model_train_v1

# LSTM Finetuned Model - Train_v0                    : 20250515_233239
eval weights/tank_game_environment_v0_20250515-233239/tank_game_environment_v0_20250515-233239.zip model_finetuned_train_v0

# LSTM Finetuned Model - Train_v0 - No Dense Rewards : 20250515_234037
eval weights/tank_game_environment_v0_20250515-234037/tank_game_environment_v0_20250515-234037.zip model_finetuned_train_v0_no_dense

# LSTM Finetuned Model - Train_v2                    : 20250515_234713
eval weights/tank_game_environment_v1_20250515-234713/tank_game_environment_v1_20250515-234713.zip model_finetuned_train_v2

# LSTM Finetuned Model - Train_v2 - No Dense Rewards : 20250515_234926
eval weights/tank_game_environment_v1_20250515-234926/tank_game_environment_v1_20250515-234926.zip model_finetuned_train_v2_no_dense
