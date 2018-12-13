#pragma once

#define DEFAULT_BOARD_WIDTH 8
#define DEFAULT_BOARD_HEIGHT 8
#define DEFAULT_INIT_X_POS 1
#define DEFAULT_INIT_Y_POS 1
#define MOVES_COUNT 8
#define MAX_STEP_BACK_COUNT 1024 * 1024

static const char DEFAULT_FILE_PATH[24] = "knight_tour_seq.txt";

static const int MOVES[MOVES_COUNT][2] = {
    {-2,  1},
    {-1,  2},
    { 1,  2},
    { 2,  1},
    { 2, -1},
    { 1, -2},
    {-1, -2},
    {-2, -1}
};

typedef enum {
    TOUR_SEARCH,
    TOUR_EXISTS,
    TOUR_NOT_EXIST
} tour_condition_t;

typedef enum {
    NEW_MOVE,
    BAD_MOVE
} move_condition_t;
