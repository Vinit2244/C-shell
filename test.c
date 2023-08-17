#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <pwd.h>

void remove_leading_and_trailing_spaces(char* str) {
    // removing leading spaces 
    int front_index = 0;
    while (str[front_index] == ' ' || str[front_index] == '\t') {
        front_index++;
    }
    int rear_index = strlen(str) - 1;
    while (str[rear_index] == ' ' || str[rear_index] == '\t') {
        rear_index--;
    }
    if (rear_index < front_index) {
        str[0] = '\0';
        return;
    }
    int len = rear_index - front_index + 1;
    char* final = (char*) malloc(sizeof(char) * (len + 1));
    for (int i = 0; i < len; i++) {
        final[i] = str[front_index++];
    }
    final[len] = '\0';
    for (int i = 0; i < len + 1; i++) {
        str[i] = final[i];
    }
    free(final);
}

char** get_list_of_commands(char* input) {
    int no_of_commands = 0;
    int q_flag = 0;
    for (int i = 0; i < strlen(input); i++) {
        if (input[i] == '"' || input[i] == '\'') {
            if (q_flag == 0) q_flag = 1;
            else q_flag = 0;
        }
        if ((input[i] == ';' || input[i] == '&') && q_flag == 0) {
            no_of_commands++;
        }
    }
    no_of_commands++;
    char** list_of_commands = (char**) malloc(sizeof(char*) * (no_of_commands + 1));
    for (int i = 0; i < no_of_commands + 1; i++) {
        list_of_commands[i] = (char*) malloc(sizeof(char) * 5000);
        list_of_commands[i][0] = '\0';
    }
    int input_str_index = 0;
    int no_of_commands_extracted = 0;
    int buffer_index = 0;
    int flag = 0;
    int quotes_flag = 0;
    while (input[input_str_index] != '\0' && no_of_commands_extracted < no_of_commands) {
        char curr_char = input[input_str_index++];
        if (curr_char == '&' && quotes_flag == 0) {
            // use store and reset function
            flag = 0;
            // buffer[buffer_index] = '&';
            list_of_commands[no_of_commands_extracted][buffer_index] = '&';
            buffer_index++;
            list_of_commands[no_of_commands_extracted][buffer_index] = '\0';
            remove_leading_and_trailing_spaces(list_of_commands[no_of_commands_extracted]);
            buffer_index = 0;
            no_of_commands_extracted++;
        } else if (curr_char == ';' && quotes_flag == 0) {
            flag = 0;
            // buffer[buffer_index] = '\0';
            list_of_commands[no_of_commands_extracted][buffer_index] = '\0';
            buffer_index = 0;
            remove_leading_and_trailing_spaces(list_of_commands[no_of_commands_extracted]);
            no_of_commands_extracted++;
        } else {
            if ((curr_char == ' ' || curr_char == '\t') && quotes_flag == 0) {
                if (flag == 0) {
                    list_of_commands[no_of_commands_extracted][buffer_index] = ' ';
                    buffer_index++;
                    flag = 1;
                } else {
                    continue;
                }
            } else {
                flag = 0;
                list_of_commands[no_of_commands_extracted][buffer_index] = curr_char;
                buffer_index++;
                if (curr_char == '"' || curr_char == '\'') {
                    if (quotes_flag == 0) quotes_flag = 1;
                    else quotes_flag = 0;
                }
            }
        }
    }
    if (no_of_commands_extracted < no_of_commands) {
        remove_leading_and_trailing_spaces(list_of_commands[no_of_commands_extracted]);
        no_of_commands_extracted++;
    }
    return list_of_commands;
}

int main(int argc, char* argv[]) {
    // char* input = (char*) calloc(5000, sizeof(char));
    // fgets(input, 4096, stdin);
    char input[5000] = " warp test ; pwd\n";
    char** list_of_commands = get_list_of_commands(input);
    int idx = 0;
    char* curr_command = list_of_commands[idx];
    while (curr_command[0] != '\0') {
        printf("Command %d: %s\n", idx + 1, curr_command);
        idx++;
        curr_command = list_of_commands[idx];
    }
    return 0;
}