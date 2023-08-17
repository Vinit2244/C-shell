#ifndef __PROMPT_H
#define __PROMPT_H

void prompt(char* home_directory);
int is_inside_home_directory(char* pwd, char* home_directory);
char* remove_leading_and_trailing_spaces(char* str);
char** get_list_of_commands(char* input);
void free_commands_list(char** list_of_commands);

#endif