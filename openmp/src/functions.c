#include "functions.h"

void print_tour_info(FILE *file, tour_info_t tour_info) {
    fprintf(file, "LOOKING A TOUR FOR:\n\n");

    fprintf(file, "BOARD WIDTH:  %d\n", tour_info.width);
    fprintf(file, "BOARD HEIGHT: %d\n", tour_info.height);
    //fprintf(file, "KNIGHT X POSITION: %d\n", tour_info.x_pos);
    //fprintf(file, "KNIGHT Y POSITION: %d\n", tour_info.y_pos);
    fprintf(file, "\n");
}

void print_board(FILE *file, path_t *path, tour_info_t tour_info) {
    for (int y = 0; y < tour_info.height; y++) {
        for (int x = 0; x < tour_info.width; x++)
            fprintf(file, "%10d ", path->board[y][x]);
        fprintf(file, "\n");
    }
    fprintf(file, "\n");
}

void print_step(step_t *step) {
    printf("CURRENT: %p\n", step);
    printf("PREVIOUS: %p\n", step->previous);
    printf("NEXT: %p\n", step->next);
    printf("POSITION: %4d %4d\n", step->cur_x_pos, step->cur_y_pos);
    printf("POSSIBLE STEPS: ");
    for (int i = 0; i < MOVES_COUNT; i++)
        printf("%d ", step->possible_steps[i]);
    printf("\n\n");
}

void print_steps(step_t *step) {
    while (step != NULL) {
        print_step(step);
        step = step->next;
    }
}

step_t *init_first_step(int x_pos, int y_pos) {
    step_t *step = NULL;

    step = (step_t *) malloc(sizeof(step_t));
    memset(step, 0, sizeof(step_t));

    step->cur_x_pos = x_pos;
    step->cur_y_pos = y_pos;
    step->move = 0;

    for (int i = 0; i < MOVES_COUNT; i++)
        step->possible_steps[i] = NEW_MOVE;

    step->previous = NULL;
    step->next = NULL;

    return step;
}

step_t *add_step(step_t *previous, int move) {
    step_t *step = NULL;

    step = (step_t *) malloc(sizeof(step_t));
    memset(step, 0, sizeof(step_t));

    step->cur_x_pos = previous->cur_x_pos + MOVES[move][0];
    step->cur_y_pos = previous->cur_y_pos + MOVES[move][1];
    step->move = move;

    for (int i = 0; i < MOVES_COUNT; i++)
        step->possible_steps[i] = 0;

    step->previous = previous;
    previous->next = step;
    step->next = NULL;

    return step;
}

void free_step(step_t *step) {
    step_t *tmp = NULL;

    while (step != NULL) {
        tmp = step;
        step = step->next;
        free(tmp);
    }
}

void free_path(path_t *path, tour_info_t tour_info) {
    for (int i = 0; i < tour_info.height; i++)
        free(path->board[i]);
    free(path->board);
    free_step(path->steps);
    free(path);
}

path_t *init_path(tour_info_t tour_info) {
    path_t *path = NULL;

    path = (path_t *) malloc(sizeof(path_t));
    memset(path, 0, sizeof(path_t));

    path->step_count = 1;

    path->board = (int **) calloc(tour_info.height, sizeof(int *));
    memset(path->board, 0, tour_info.height * sizeof(int *));
    for (int i = 0; i < tour_info.height; i++) {
        path->board[i] = (int *) calloc(tour_info.width, sizeof(int));
        memset(path->board[i], 0, tour_info.width * sizeof(int));
    }

    step_t *first_step = init_first_step(tour_info.x_pos - 1, tour_info.y_pos - 1);

    path->board[tour_info.y_pos - 1][tour_info.x_pos - 1] = 1;

    path->cur_step = first_step;
    path->steps = first_step;

    return path;
}

tour_condition_t step_back(step_t *cur_step, path_t *path) {
    if (cur_step->previous == NULL)
        return TOUR_NOT_EXIST;
    path->board[cur_step->cur_y_pos][cur_step->cur_x_pos] = 0;
    cur_step = cur_step->previous;
    cur_step->possible_steps[cur_step->next->move] = BAD_MOVE;
    free_step(cur_step->next);
    cur_step->next = NULL;
    path->cur_step = cur_step;
    path->step_count--;
    return TOUR_SEARCH;
}

int valid(int x_pos, int y_pos, int **board, int width, int height, int move) {
    int next_x_pos = x_pos + MOVES[move][0];
    int next_y_pos = y_pos + MOVES[move][1];
    int invalid_board_place = (next_x_pos < 0 || next_x_pos >= width
                        || next_y_pos < 0 || next_y_pos >= height
                        || board[next_y_pos][next_x_pos]);

    return !invalid_board_place;
}

int compare(const void *a, const void *b) {
    return ((int *) a)[0] - ((int *) b)[0];
}

int get_next_move_wansdorf(step_t *cur_step, int **board, int width, int height, tour_info_t tour_info, path_t *path) {
    int valids[MOVES_COUNT][2];

    int x_pos = cur_step->cur_x_pos;
    int y_pos = cur_step->cur_y_pos;

    for (int i = 0; i < MOVES_COUNT; i++) {
        valids[i][0] = 0;
        valids[i][1] = -1;
        if (valid(x_pos, y_pos, board, width, height, i) && cur_step->possible_steps[i] == NEW_MOVE) {
            valids[i][1] = i;
            for (int j = 0; j < MOVES_COUNT; j++)
                if (valid(x_pos + MOVES[i][0], y_pos + MOVES[i][1], board, width, height, j))
                    valids[i][0]++;
        }
    }

    // int a = 0;
    // a = scanf("%d", &a);

    // print_board(stdout, path, tour_info);

    qsort(&valids[0][0], MOVES_COUNT, 2 * sizeof(int), &compare);

    for (int i = 0; i < MOVES_COUNT; i++)
        if (valids[i][1] != -1)
            return valids[i][1];

    return -1;
}

tour_condition_t make_step(path_t *path, tour_info_t tour_info) {

    static long long int step_back_count = 0;

    step_t *cur_step = path->cur_step;
    int width = tour_info.width;
    int height = tour_info.height;
    int x_pos = cur_step->cur_x_pos;
    int y_pos = cur_step->cur_y_pos;
    int **board = path->board;

    /* Проверка на успех */
    if (path->step_count == width * height) {
        board[y_pos][x_pos] = path->step_count;
        return TOUR_EXISTS;
    }

    /* Выбор следующего хода */
    int next_move = get_next_move_wansdorf(cur_step, path->board, width, height, tour_info, path);

    /* Шаг назад, если нет следующего шага */
    if (next_move == -1) {
        if (step_back_count > MAX_STEP_BACK_COUNT) {
            step_back_count = 0;
            return TOUR_NOT_EXIST;
        }
        step_back_count++;
        return step_back(cur_step, path);
    }

    /* Шаг вперед */
    cur_step->next = add_step(cur_step, next_move);
    cur_step = cur_step->next;
    path->cur_step = cur_step;
    path->step_count++;
    board[cur_step->cur_y_pos][cur_step->cur_x_pos] = path->step_count;

    return TOUR_SEARCH;
}

int find_path(tour_info_t tour_info, FILE *file) {
    int tour_exists = 0;
    int tour_not_exist = 0;
    tour_condition_t condition = TOUR_SEARCH;
    path_t *path = NULL;

    path = init_path(tour_info);

    while (!tour_exists && !tour_not_exist) {
        condition = make_step(path, tour_info);
    
        if (condition == TOUR_EXISTS) {
            tour_exists = 1;
            print_board(file, path, tour_info);
        }
        else if (condition == TOUR_NOT_EXIST)
            tour_not_exist = 1;
    }

    free_path(path, tour_info);
    path = NULL;

    return tour_exists;
}

int find_tour(tour_info_t tour_info, const char *file_path) {
    int tour_exists = 0;
    FILE *file = fopen(file_path, "w");

    print_tour_info(file, tour_info);

    tour_exists = find_path(tour_info, file);

    if (!tour_exists)
        fprintf(file, "TOUR NOT EXISTS");
    fclose(file);

    return tour_exists;
}
