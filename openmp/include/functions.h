#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "constants.h"
#include "types.h"

int find_tour(tour_info_t tour_info, const char *file_path);
void print_tour_info(FILE *file, tour_info_t tour_info);
