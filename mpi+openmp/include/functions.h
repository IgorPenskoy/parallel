#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "constants.h"
#include "types.h"

int **find_tour(tour_info_t tour_info);
void print_tour_info(FILE *file, tour_info_t tour_info);
void print_board(FILE *file, int **board, tour_info_t tour_info);
void free_board(int **board, int height);
