#ifndef H_WRITE_LIB
#define H_WRITE_LIB

#include "common_and_config.h"


double write_file(char *buffer, long long size, long long offset, enum single_file type);

double write_multiple_files(long long file_size, int process_number);

#endif