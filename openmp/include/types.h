#pragma once

#include "constants.h"

typedef struct tour_info_t {
    int width;
    int height;
    int x_pos;
    int y_pos;
} tour_info_t;

typedef struct step_t {
    int cur_x_pos;
    int cur_y_pos;
    int move;
    int possible_steps[MOVES_COUNT];
    struct step_t *previous;
    struct step_t *next;
} step_t;

typedef struct path_t {
    int step_count;
    int **board;
    step_t *cur_step;
    step_t *steps;
} path_t;
