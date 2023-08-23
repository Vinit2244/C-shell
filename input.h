#ifndef __INPUT_H
#define __INPUT_H
#include "linked_list.h"

extern int bg_process;

void input(char* command, char* home_directory, char* cwd, char* prev_dir, int store, char* last_command, int* t);
void handle_passed(int sig, siginfo_t *info, void* context);
void handle_failed(int sig, siginfo_t *info, void* context);


#endif