#ifndef __UTILITIES_H
#define __UTILITIES_H

extern char* global_buffer;
extern int global_buffer_empty;
extern int global_fg_pid;
extern char* fg_command_name;

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
void convert_to_int(char* number, int* num, int* flag, int ap, int w);
int is_pipe_present(char* string);
int is_write_present(char* string);
int is_append_present(char* string, char* app_cmd, char* to_file);
int is_input_present(char* string);

#endif