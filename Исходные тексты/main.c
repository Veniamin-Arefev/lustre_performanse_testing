#include <mpich/mpi.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "common_and_config.h"

enum {
    FILE_NAME_INDEX = 1,
    FILE_SIZE_INDEX = 2,
    TEST_NUMBER_INDEX_START = 3,
};

int main(int argc, char **argv) {
    long long file_size = 1024;
    if (argc < 3) {
        printf("Too few arguments!\n");
        exit(1);
    } else {
        set_filename(argv[FILE_NAME_INDEX]);
        file_size = strtoll(argv[FILE_SIZE_INDEX], NULL, 0);
        if (file_size < 0 || (file_size % 64 != 0)) {
            printf("Size arguments should be greater than 0 and must be divisible by 64.\n");
            exit(1);
        }
        for (int i = TEST_NUMBER_INDEX_START; i < argc; ++i) {
            for (int j = 0; argv[i][j] != '\0'; ++j) {
                switch (argv[i][j]) {
                    case '1':
                        tests_to_be_performed[0] = 1;
                        break;
                    case '2':
                        tests_to_be_performed[1] = 1;
                        break;
                    case '3':
                        tests_to_be_performed[2] = 1;
                        break;
                    case '4':
                        tests_to_be_performed[3] = 1;
                        break;
                    case '5':
                        tests_to_be_performed[4] = 1;
                        break;
                    case '6':
                        tests_to_be_performed[5] = 1;
                        break;
                    default:
                        printf("Unrecognized parameter detected!\n");
                        exit(1);
                }
            }
        }
        int is_empty = 1;
        for (int i = 0; i < 6; ++i) {
            if (tests_to_be_performed[i] != 0) {
                is_empty = 0;
            }
        }
        if (is_empty) {
            for (int i = 0; i < 6; ++i) {
                tests_to_be_performed[i] = 1;
            }
        }
    }

    MPI_Init(&argc, &argv);
    int process_number, process_count;
    MPI_Comm_rank(MPI_COMM_WORLD, &process_number);
    MPI_Comm_size(MPI_COMM_WORLD, &process_count);

    double *elapsed = perform_test_n_times(file_size, process_number, process_count, 5);
    double *write_elapsed = elapsed;
    double *read_elapsed = elapsed + 6;

    if (process_number == 0) {
        printf("Write times:\n");
        for (int i = 0; i < 6; ++i) {
            printf("%5.5f ", write_elapsed[i]);
        }
        printf("\n");
        printf("Read times:\n");
        for (int i = 0; i < 6; ++i) {
            printf("%5.5f ", read_elapsed[i]);
        }
        printf("\n");
    }

    MPI_Finalize();
    return 0;
}