#ifndef __PASTEVENTS_H
#define __PASTEVENTS_H
#include "linked_list.h"

void store_command(char* command);
void pastevents();
void purge();
int execute(int num, char* home_dir, char* cwd, char* prev_dir, int store, char* last_command, int* t);

#endif