#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "constants.h"
#include "functions.h"
#include "types.h"

int main(int argc, char const *argv[]) {
    
    tour_info_t tour_info;
 
    tour_info.width = DEFAULT_BOARD_WIDTH;
    tour_info.height = DEFAULT_BOARD_HEIGHT;
    tour_info.x_pos = DEFAULT_INIT_X_POS;
    tour_info.y_pos = DEFAULT_INIT_Y_POS;
    char *file_path = (char *) DEFAULT_FILE_PATH;

    switch (argc) {
        case 1:
            break;
        case 2:
            tour_info.width = atoi(argv[1]);
            break;
        case 3:
            tour_info.width = atoi(argv[1]);
            tour_info.height = atoi(argv[2]);
            break;
        case 4:
            tour_info.width = atoi(argv[1]);
            tour_info.height = atoi(argv[2]);
            tour_info.x_pos = atoi(argv[3]);
            break;
        case 5:
            tour_info.width = atoi(argv[1]);
            tour_info.height = atoi(argv[2]);
            tour_info.x_pos = atoi(argv[3]);
            tour_info.y_pos = atoi(argv[4]);
            break;
        case 6:
            tour_info.width = atoi(argv[1]);
            tour_info.height = atoi(argv[2]);
            tour_info.x_pos = atoi(argv[3]);
            tour_info.y_pos = atoi(argv[4]);
            file_path = (char *) argv[5];
            break;
        default:
            printf("USAGE: ./knight_tour_seq.out [width [height [x_pos [y_pos [filename ]]]]]\n");
            return 0;
    }

    if (tour_info.width > 1000 || tour_info.width < 1
        || tour_info.height > 1000 || tour_info.height < 1
        || tour_info.x_pos < 1 || tour_info.x_pos > tour_info.width
        || tour_info.y_pos < 1 || tour_info.y_pos > tour_info.height) {
            printf("WIDTH and HEIGHT must be in [1; 1000]\nX and Y positions must be inside the board\n");
            return 0;
        }

    print_tour_info(stdout, tour_info);

    int tour_exists = 0;
    volatile int flag = 0;

    clock_t start = clock();

    if (argc <= 3)
    {
        #pragma omp parallel num_threads(8) shared(flag)
        {
            #pragma omp for
            for (int i = 0; i < tour_info.width; i++) {
                for (int j = 0; j < tour_info.height; j++) {
                    if (!flag) {
                        tour_info.x_pos = i + 1;
                        tour_info.y_pos = j + 1;
                        if (find_tour(tour_info, file_path)) {
                            tour_exists = 1;
                            flag = 1;
                        }
                    }
                }
            }
        }
    }
    else
        tour_exists = find_tour(tour_info, file_path);

    clock_t end = clock();

    if (tour_exists) {
        // printf("X: %d; Y: %d\n", tour_info.x_pos, tour_info.y_pos);
        printf("TOUR EXISTS!\n");
    }
    else
        printf("TOUR NOT EXISTS\n");

    printf("TIME (S): %f\n", (double)(end - start) / CLOCKS_PER_SEC);

    return 0;
}
