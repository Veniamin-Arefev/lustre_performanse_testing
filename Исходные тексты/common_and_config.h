#ifndef H_COMMON_AND_CONFIG
#define H_COMMON_AND_CONFIG

enum single_file {
    SINGLE_FILE_COLLECTIVE_ASYNC,
    SINGLE_FILE_NON_COLLECTIVE_ASYNC,
    SINGLE_FILE_COLLECTIVE_NON_ASYNC,
    SINGLE_FILE_NON_COLLECTIVE_NON_ASYNC,
};

enum special {
    SINGLE_FILE_RANDOM,
    MULTIPLE_FILES,
};


extern const char *DIRECTORY_FOR_RW;
extern const char *PATH_FOR_RW;
extern int buffer_step;
extern int tests_to_be_performed[6];


void check_error_code(int err);

int get_random_process_number(int process_number, int process_count);

long long get_buffer_size(long long file_size, int process_number, int process_count);

long long get_offset(long long file_size, int process_number, int process_count);

void set_filename(char *filename);

char *get_multiple_files_filename(int process_number);

char *create_buffer(int unsigned long long, int rank);

double *perform_test(long long file_size, int process_number, int process_count);

double *perform_test_n_times(long long file_size, int process_number, int process_count, int n);

#endif