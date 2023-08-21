#ifndef __INPUT_H
#define __INPUT_H
#include "linked_list.h"

void input(char* command, char* home_directory, char* cwd, char* prev_dir, int store, char* last_command, int* t);
void handle_child_signal(int signum, siginfo_t *info, void *context);

#endif