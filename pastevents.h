#ifndef __PASTEVENTS_H
#define __PASTEVENTS_H

void store_command(char* command);
void pastevents();
int purge(int ap, int w);
int execute(int num, int store, int ap, int w);

#endif