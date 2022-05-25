#include <mpich/mpi.h>
#include <sys/time.h>
#include <malloc.h>

#include "common_and_config.h"


double read_file(char *buffer, long long size, long long offset, enum single_file type) {
    struct timeval begin, end;
    gettimeofday(&begin, 0);

    MPI_File fh;
    MPI_Request request;
    int ret = 0;
    ret = MPI_File_open(MPI_COMM_WORLD, PATH_FOR_RW, MPI_MODE_RDONLY, MPI_INFO_NULL, &fh);
    check_error_code(ret);

    switch (type) {
        case SINGLE_FILE_COLLECTIVE_ASYNC:
            MPI_File_iread_at_all(fh, offset, buffer, size, MPI_CHAR, &request);
            ret = MPI_Wait(&request, MPI_STATUS_IGNORE);
            break;
        case SINGLE_FILE_NON_COLLECTIVE_ASYNC:
            MPI_File_iread_at(fh, offset, buffer, size, MPI_CHAR, &request);
            ret = MPI_Wait(&request, MPI_STATUS_IGNORE);
            break;
        case SINGLE_FILE_COLLECTIVE_NON_ASYNC:
            ret = MPI_File_read_at_all(fh, offset, buffer, size, MPI_CHAR, MPI_STATUS_IGNORE);
            break;
        case SINGLE_FILE_NON_COLLECTIVE_NON_ASYNC:
            ret = MPI_File_read_at(fh, offset, buffer, size, MPI_CHAR, MPI_STATUS_IGNORE);
            break;
    }
    check_error_code(ret);

    MPI_File_close(&fh);

    gettimeofday(&end, 0);
    long seconds = end.tv_sec - begin.tv_sec;
    long microseconds = end.tv_usec - begin.tv_usec;
    double elapsed = seconds + microseconds * 1e-6;

    double max;
    MPI_Reduce(&elapsed, &max, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    return max;
}

double read_multiple_files(long long file_size, int process_number) {
    struct timeval begin, end;
    gettimeofday(&begin, 0);

    MPI_File fh;
    int ret = 0;

    char *filename = get_multiple_files_filename(process_number);
    ret = MPI_File_open(MPI_COMM_SELF, filename, MPI_MODE_RDONLY, MPI_INFO_NULL, &fh);
    check_error_code(ret);
    free(filename);

    char *str = malloc(sizeof(char) * file_size + 1);
    ret = MPI_File_read_at(fh, 0, str, 0, MPI_CHAR, MPI_STATUS_IGNORE);
    check_error_code(ret);


    MPI_File_close(&fh);

    gettimeofday(&end, 0);
    long seconds = end.tv_sec - begin.tv_sec;
    long microseconds = end.tv_usec - begin.tv_usec;
    double elapsed = seconds + microseconds * 1e-6;

    double max;
    MPI_Reduce(&elapsed, &max, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    return max;
}