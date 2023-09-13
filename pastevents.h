#ifndef __PASTEVENTS_H
#define __PASTEVENTS_H

int history(char** argument_tokens, int no_of_arguments, int* pastevents_present, int ap, int w, int ip, char* curr_command, char* input_file_name_redirection, int store, int* pastevents_execute_present, char* input_string);
int store_command(char* command);
int pastevents();
int purge(int ap, int w);
int execute(int num, int store, int ap, int w, char* curr_command, int* is_pastevents_execute_present, char* input_string);

#endif