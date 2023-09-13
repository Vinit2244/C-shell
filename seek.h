#ifndef __SEEK_FILE_H
#define __SEEK_FILE_H
#include "LL2.h"

int find_file_dir(char** argument_tokens, int no_of_arguments, int w, int ap, int ip, char* input_file_name_redirection);
void seek(char* path_to_base_dir, char* file_name, linked_list_head paths);

#endif