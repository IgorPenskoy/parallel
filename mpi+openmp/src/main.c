#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <mpi.h>
#include <omp.h>

#include "constants.h"
#include "types.h"
#include "functions.h"

int main(int argc, char *argv[]) {

    tour_info_t tour_info; // информация о входных данных пути обхода
    tour_info.width = DEFAULT_BOARD_WIDTH; // ширина по умолчанию
    tour_info.height = DEFAULT_BOARD_HEIGHT; // высота по умолчанию
    char *file_path = (char *) DEFAULT_FILE_PATH; // имя выходного файла по умолчанию

    // обработка аргументов командной строки
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
            file_path = argv[3];
            break;
        default:
            printf("USAGE: ./knight_tour_seq.out [width [height [filename]]]\n");
            return 0;
    }

    int myrank = 0; // номер текущего процесса
    int nprocs = 0; // кол-во процессов
    int len = 0; // кол-во логических процессоров
    char name[MPI_MAX_PROCESSOR_NAME]; // имя текущего процесса

    // Инициализация MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Get_processor_name(name, &len);

    MPI_Barrier(MPI_COMM_WORLD);
    double start = MPI_Wtime();

    // проверка входных данных и печать информации о размерах доски
    if (myrank == 0) {
        if (tour_info.width > 1000 || tour_info.width < 1
            || tour_info.height > 1000 || tour_info.height < 1) {
            printf("WIDTH and HEIGHT must be in [1; 1000]\n");
            MPI_Barrier(MPI_COMM_WORLD);
            MPI_Finalize();
            return 0;
        }
        print_tour_info(stdout, tour_info);
    }

    int tour_exists = 0; // флаг сущестоввания пути обхода
    int **board = NULL;

    // Вычисление границ обрабатываемой области для процесса
    int size = tour_info.width / nprocs;
    if (size == 0)
        size = 1;
    int left_border = size * myrank >= tour_info.width ? tour_info.width - 1 : size * myrank;
    int right_border = left_border + size > tour_info.width ? tour_info.width : left_border + size;
    if (myrank == nprocs - 1)
        right_border = tour_info.width;

    // основной цикл по столбцам
    for (int i = left_border; i < right_border && !tour_exists; i++) {
        int j = 0;
        // цикл по строкам
        while (j < tour_info.height && !tour_exists) {
            int omp_thread_count = 1;
            // параллельная обработка строк

            /***** OpenMP *****/
            #pragma omp numthreads(8) parallel
            {
                omp_thread_count = omp_get_num_threads();
                #pragma omp for
                for (int k = 0; k < omp_thread_count; k++) {
                    if (j + k < tour_info.height && !tour_exists) {
                        tour_info.x_pos = i;
                        tour_info.y_pos = j + k;
                        board = find_tour(tour_info);
                        if (board)
                            tour_exists = 1;
                    }
                }
            }
            /***** OpenMP *****/

            // решение о завершении работы процессов
            int flags[nprocs];
            memset(flags, 0, nprocs);
            MPI_Allgather(&tour_exists, 1, MPI_INT, flags, 1, MPI_INT, MPI_COMM_WORLD);
            for (int i = 0; i < nprocs; i++)
                if (flags[i] == 1) {
                    tour_exists = 1;
                    if (myrank == i) {
                        FILE *file = fopen(file_path, "w");
                        print_tour_info(file, tour_info);
                        fprintf(file, "TOUR EXISTS!\n");
                        fprintf(file, "INIT X: %d; INIT Y: %d\n\n", tour_info.x_pos + 1, tour_info.y_pos + 1);
                        print_board(file, board, tour_info);
                        fclose(file);
                        printf("TOUR EXISTS!\n");
                        printf("INIT X: %d; INIT Y: %d\n", tour_info.x_pos + 1, tour_info.y_pos + 1);
                    }
                    free_board(board, tour_info.height);
                    break;
                }
            j += omp_thread_count;
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    double end = MPI_Wtime();

    if (myrank == 0) {
        printf("TIME (S): %10.5lf\n", end - start);
        if (!tour_exists)
            printf("TOUR NOT EXIST!\n");
    }

    MPI_Finalize();
    return 0;
}
