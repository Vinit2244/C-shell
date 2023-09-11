#ifndef __PEEK_H
#define __PEEK_H

int peek(char* path, int a, int l, char* cwd, char* home_dir, char* prev_dir, int ap, int w);
char** generate_sorted_file_list(char* path);
int print_extra_details(char* complete_path_of_file, char** files_list, int idx, int ap, int w);

#endif