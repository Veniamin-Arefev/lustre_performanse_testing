#ifndef H_READ_LIB
#define H_READ_LIB

#include "common_and_config.h"


double read_file(char *buffer, long long size, long long offset, enum single_file type);

double read_multiple_files(long long file_size, int process_number);

#endif