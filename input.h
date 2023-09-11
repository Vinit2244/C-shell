#ifndef __INPUT_H
#define __INPUT_H
#include "linked_list.h"

extern int bg_process;

void input(char* command, char* home_directory, char* cwd, char* prev_dir, int store, char* last_command, int* t, int w, int ap, int ip, char* output_file_name_redirection, char* input_file_name_redirection, int inp_from_file, int inp_from_pipe, int output_to_file, int output_to_pipe, int read_fd, int write_fd);

#endif