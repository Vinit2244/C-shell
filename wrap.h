#ifndef __WRAP2_H
#define __WRAP2_H

void wrap(char* cwd, char* path, char* prev_dir, char* home_dir);
int check_if_dir(char* cwd, char* file_name);
void check_if_such_dir_exists(char* cwd, int* flag, char* next_dir);
void update_cwd(char* cwd, char* next_dir);

#endif