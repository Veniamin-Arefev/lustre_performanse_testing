#include <mpich/mpi.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <limits.h>


#include "write_lib.h"
#include "read_lib.h"

const char *DIRECTORY_FOR_RW = "/tmp/test/";
const char *PATH_FOR_RW = NULL;
int buffer_step = 64;
int tests_to_be_performed[]  = {0, 0, 0, 0, 0, 0};

void check_error_code(int err) {
    if (err != MPI_SUCCESS) {
        int errorStringLen;
        char errorString[MPI_MAX_ERROR_STRING];
        MPI_Error_string(err, errorString, &errorStringLen);
        printf("ERROR!!! (%s)\n", errorString);
    }
}

int get_random_process_number(int process_number, int process_count) {

    int *random_processes = NULL;
    if (process_number == 0) {
        random_processes = malloc(sizeof(int) * process_count);
        for (int i = 0; i < process_count; ++i) {
            random_processes[i] = i;
        }
        srand(time(NULL));
        int index, elem;
        for (int i = 0; i < process_count; ++i) {
            index = rand() % process_count;
            elem = random_processes[index];
            random_processes[index] = random_processes[i];
            random_processes[i] = elem;
        }
    }

    int proc;

    MPI_Scatter(random_processes, 1, MPI_INT, &proc, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (process_number == 0) {
        free(random_processes);
    }

    return proc;
}

long long get_buffer_size(long long file_size, int process_number, int process_count) {
    long long lines_count = file_size / buffer_step;
    long long proc_lines = lines_count / process_count + ((lines_count % process_count) > process_number ? 1 : 0);

    return proc_lines * buffer_step;
}

long long get_offset(long long file_size, int process_number, int process_count) {
    long long lines_count = file_size / buffer_step;
    long long proc_offset = (lines_count / process_count) * process_number +
                            ((lines_count % process_count) > process_number ? process_number : lines_count %
                                                                                               process_count);
    return proc_offset * buffer_step;
}

void set_filename(char *filename) {
    char *str = malloc(sizeof(char) * PATH_MAX);
    sprintf(str, "%s%s", DIRECTORY_FOR_RW, filename);
    PATH_FOR_RW = str;
}

char *get_multiple_files_filename(int process_number) {
    char *str = malloc(sizeof(char) * 65);
    sprintf(str, "%s%d", PATH_FOR_RW, process_number);
    return str;
}

void delete_multiple_files(int process_number) {
    char *filename = get_multiple_files_filename(process_number);
    MPI_File_delete(filename, MPI_INFO_NULL);
    free(filename);
}

char *create_buffer(unsigned long long size, int rank) {
    if (size % buffer_step != 0) {
        return NULL;
    }
    char *str = malloc(sizeof(char) * size + 1);

    for (unsigned long long i = 0; i < size; i += buffer_step) {
        sprintf(str + i, "Hello, World! My rank is %*d\n", 38, rank);
    }

    return str;
}

double *perform_test(long long file_size, int process_number, int process_count) {
    long long str_size = get_buffer_size(file_size, process_number, process_count);
    long long offset = get_offset(file_size, process_number, process_count);
    char *str = create_buffer(str_size, process_number);

    double *elapsed = malloc(sizeof(double) * 12);
    double *write_elapsed = elapsed;
    double *read_elapsed = elapsed + 6;

    for (int i = 0; i < 12; ++i) {
        elapsed[i] = 0;
    }

    if (tests_to_be_performed[0]) {
        write_elapsed[0] = write_file(str, str_size, offset, SINGLE_FILE_COLLECTIVE_ASYNC);
        read_elapsed[0] = read_file(str, str_size, offset, SINGLE_FILE_COLLECTIVE_ASYNC);
        MPI_File_delete(PATH_FOR_RW, MPI_INFO_NULL);
    }

    if (tests_to_be_performed[1]) {
        write_elapsed[1] = write_file(str, str_size, offset, SINGLE_FILE_NON_COLLECTIVE_ASYNC);
        read_elapsed[1] = read_file(str, str_size, offset, SINGLE_FILE_NON_COLLECTIVE_ASYNC);
        MPI_File_delete(PATH_FOR_RW, MPI_INFO_NULL);
    }

    if (tests_to_be_performed[2]) {
        write_elapsed[2] = write_file(str, str_size, offset, SINGLE_FILE_COLLECTIVE_NON_ASYNC);
        read_elapsed[2] = read_file(str, str_size, offset, SINGLE_FILE_COLLECTIVE_NON_ASYNC);
        MPI_File_delete(PATH_FOR_RW, MPI_INFO_NULL);
    }

    if (tests_to_be_performed[3]) {
        write_elapsed[3] = write_file(str, str_size, offset, SINGLE_FILE_NON_COLLECTIVE_NON_ASYNC);
        read_elapsed[3] = read_file(str, str_size, offset, SINGLE_FILE_NON_COLLECTIVE_NON_ASYNC);
        MPI_File_delete(PATH_FOR_RW, MPI_INFO_NULL);
    }

    free(str);

    if (tests_to_be_performed[4]) {
        int random_process_number = get_random_process_number(process_number, process_count);
        str_size = get_buffer_size(file_size, random_process_number, process_count);
        offset = get_offset(file_size, random_process_number, process_count);
        str = create_buffer(str_size, process_number);
        write_elapsed[4] = write_file(str, str_size, offset, SINGLE_FILE_NON_COLLECTIVE_NON_ASYNC);
        read_elapsed[4] = read_file(str, str_size, offset, SINGLE_FILE_NON_COLLECTIVE_NON_ASYNC);
        MPI_File_delete(PATH_FOR_RW, MPI_INFO_NULL);
        free(str);
    }

    if (tests_to_be_performed[5]) {
        write_elapsed[5] = write_multiple_files(file_size, process_number);
        read_elapsed[5] = read_multiple_files(file_size, process_number);
        delete_multiple_files(process_number);
    }

    return elapsed;
}

double *perform_test_n_times(long long file_size, int process_number, int process_count, int n) {
    double **all_tests = malloc(sizeof(double **) * n);
    for (int i = 0; i < n; ++i) {
        all_tests[i] = perform_test(file_size, process_number, process_count);
    }

    double *result = malloc(sizeof(double) * 12);
    for (int j = 0; j < 12; ++j) {
        result[j] = 0;
    }
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < 12; ++j) {
            result[j] += all_tests[i][j];
        }
    }
    for (int j = 0; j < 12; ++j) {
        result[j] /= n;
    }

    for (int i = 0; i < n; ++i) {
        free(all_tests[i]);
    }

    free(all_tests);

    return result;
}