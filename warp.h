#ifndef __WARP_H
#define __WARP_H

void warp(char* cwd, char* path, char* prev_dir, char* home_dir);
int check_if_dir(char* path, char* file_name);
int check_if_such_dir_exists(char* path, char* next_dir);
void update_path(char* path, char* next_dir);
char** generate_tokens(char* str, char c);
void free_tokens(char** tokens);

#endif