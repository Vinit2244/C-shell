#ifndef __UTILITIES_H
#define __UTILITIES_H

extern char* global_buffer;
extern int global_buffer_empty;

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
char* relative_path(char* complete_path, char* base_dir_path);
void print_global_buffer_onto_terminal();
void bprintf(char* buffer, char* string);
// void write_in_global_buffer(char* string_to_be_written);

#endif