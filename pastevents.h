#ifndef __PASTEVENTS_H
#define __PASTEVENTS_H
#include "linked_list.h"

void store_command(char* command, char* home_dir);
void pastevents(char* home_dir);
void purge(int ap, int w, char* home_dir);
int execute(int num, char* home_dir, char* cwd, char* prev_dir, int store, char* last_command, int* t, int ap, int w);

#endif