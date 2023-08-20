#ifndef __UTILITIES_H
#define __UTILITIES_H

void free_tokens(char** tokens);
char** generate_tokens(char* str, char c);
int check_if_dir(char* path, char* file_name);
int check_if_such_dir_exists(char* path, char* next_dir);
void update_path(char* path, char* next_dir);
void remove_leading_and_trailing_spaces(char* str);
char* generate_new_path(char* cwd, char* path, char* prev_dir, char* home_dir);
void sort_strings(char** strings, int no_of_strings);
int str_to_int(char* str);
char* remove_extension(char* file_name);

#endif