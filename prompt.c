#include "headers.h"

void prompt(char* home_directory) {
    // Do not hardcode the prmopt
    register struct passwd *pw;
    pw = getpwuid(getuid());
    char* username = pw->pw_name;
    if (username == NULL) {
        perror("Error: ");
    } else {
        char* hostname = (char*) malloc(sizeof(char) * 10000);
        gethostname(hostname, 9999);

        char* pwd = (char*) malloc(sizeof(char) * 10000);
        getcwd(pwd, 9999);
        if (pwd == NULL) {
            perror("Error: ");
        } else {
            printf("<");
            int index_of_difference = is_inside_home_directory(pwd, home_directory); // returns -1 if we are outside the home directory, returns -2 if pwd == home directory else returns the index from where the current directory path starts to differ from our home directory
            if (index_of_difference != -1 && index_of_difference != -2) {
                // print the relative path
                //========================= Method 1 =========================
                char* relative_path = &pwd[index_of_difference];
                printf("%s@%s:~%s", username, hostname, relative_path);
                //========================= Method 2 =========================
                /*
                char* relative_path = (char*) malloc(sizeof(char) * strlen(pwd)); // relative path can never be longer than absolute path
                relative_path[0] = '~';
                relative_path[1] = '/';
                int i;
                for (i = 2; pwd[index_of_difference] != '\0'; i++) {
                    relative_path[i] = pwd[index_of_difference++];
                }
                relative_path[i] = '\0';
                printf("%s@%s:%s", username, relative_path);
                free(relative_path);
                */
            } else if (index_of_difference == -2) {
                printf("%s@%s:~", username, hostname);
            } else {
                // print the absolute path
                printf("%s@%s:%s", username, hostname, pwd);
            }
            printf(">"); 
        }
        free(hostname);
        free(pwd);
    }
}

int is_inside_home_directory(char* pwd, char* home_directory) {
    int index = 0;
    while(pwd[index] != '\0' && home_directory[index] != '\0') {
        if (pwd[index] != home_directory[index]) {
            break;
        }
        index++;
    }
    if (pwd[index] == '\0' && home_directory[index] == '\0') {
        return -2;
    } else if (pwd[index] != '\0' && home_directory[index] == '\0') {
        return index;
    } else {
        return -1;
    }
}

char* remove_leading_and_trailing_spaces(char* str) {
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
        free(str);
        return NULL;
    }
    int len = rear_index - front_index + 1;
    char* final = (char*) malloc(sizeof(char) * (len + 1));
    for (int i = 0; i < len; i++) {
        final[i] = str[front_index++];
    }
    final[len] = '\0';
    free(str);
    return final;
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
        list_of_commands[i] = NULL;
    }
    int input_str_index = 0;
    int no_of_commands_extracted = 0;
    char* buffer = (char*) malloc(sizeof(char) * 5000);
    int buffer_index = 0;
    int flag = 0;
    int quotes_flag = 0;
    while (input[input_str_index] != '\0' && no_of_commands_extracted < no_of_commands) {
        char curr_char = input[input_str_index++];
        if (curr_char == '&' && quotes_flag == 0) {
            // use store and reset function
            flag = 0;
            buffer[buffer_index] = '&';
            buffer_index++;
            buffer[buffer_index] = '\0';
            list_of_commands[no_of_commands_extracted++] = remove_leading_and_trailing_spaces(buffer);
            buffer_index = 0;
            buffer = (char*) malloc(sizeof(char) * 5000);
        } else if (curr_char == ';' && quotes_flag == 0) {
            flag = 0;
            buffer[buffer_index] = '\0';
            buffer_index = 0;
            list_of_commands[no_of_commands_extracted++] = remove_leading_and_trailing_spaces(buffer);
            buffer = (char*) malloc(sizeof(char) * 5000);
        } else {
            if ((curr_char == ' ' || curr_char == '\t') && quotes_flag == 0) {
                if (flag == 0) {
                    buffer[buffer_index++] = ' ';
                    flag = 1;
                } else {
                    continue;
                }
            } else {
                flag = 0;
                buffer[buffer_index++] = curr_char;
                if (curr_char == '"' || curr_char == '\'') {
                    if (quotes_flag == 0) quotes_flag = 1;
                    else quotes_flag = 0;
                }
            }
        }
    }
    if (no_of_commands_extracted < no_of_commands) {
        list_of_commands[no_of_commands_extracted++] = remove_leading_and_trailing_spaces(buffer);
    }
    return list_of_commands;
}

void free_commands_list(char** list_of_commands) {
    int idx = 0;
    char* curr_command = list_of_commands[idx];
    while (curr_command != NULL) {
        free(curr_command);
        idx++;
        curr_command = list_of_commands[idx];
    }
    free(list_of_commands);
}