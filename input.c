#include "headers.h"

void input(char* command, char* home_directory, char* cwd, char* prev_dir, int store) {
    int overall_success = 1;
    int pastevents_present = 0;
    char* input = (char*) calloc(5000, sizeof(char));
    if (command == NULL) {
        store = 1;
        // Print appropriate prompt with username, systemname and directory before accepting input
        prompt(home_directory, cwd);
        
        fgets(input, 4096, stdin);
        input[strlen(input)] = '\0';

        for (int i = 0; i < strlen(input); i++) {
            if (input[i] != '\n') continue;
            else {
                input[i] = '\0';
            }
        }
    } else {
        store = 0;
        for (int i = 0; i < strlen(command); i++) {
            input[i] = command[i];
        }
        input[strlen(command)] = '\0';
    }

    char** list_of_commands = get_list_of_commands(input); // contains the list of all commands as separate strings in the form of a 2D array
        
    int idx = 0;
    char* curr_command = list_of_commands[idx];
    while (curr_command[0] != '\0') {

        // ================= Printing the commands =================
        // printf("Command %d: %s\n", idx + 1, curr_command);

        // Different commands
// ===================================================================================
        // warp
        char warp_str[5] = "warp";
        int warp_flag = 1;

        for (int i = 0; i < 4; i++) {
            if (warp_str[i] == curr_command[i]) continue;
            else {
                warp_flag = 0;
                break;
            }
        }

        // checking if warp command is present
        if (warp_flag) {
            char** path_tokens = generate_tokens(curr_command, ' ');
            int no_of_arguments = 0;
            while(path_tokens[no_of_arguments] != NULL) {
                no_of_arguments++;
            }
            no_of_arguments--;

            int success = 1;
            if (no_of_arguments == 0) {
                char* c = "~";
                int exit_code = warp(cwd, c, prev_dir, home_directory);
                if (exit_code == 0) success = 0;
            } else {
                for (int i = 1; i <= no_of_arguments; i++) {
                    int exit_code = warp(cwd, path_tokens[i], prev_dir, home_directory);
                    if (exit_code == 0) success = 0;
                }
            }
            if (success == 0) overall_success = 0;
            // if (success && store) store_command(curr_command);
            free_tokens(path_tokens);
        }

// ===================================================================================
        // peek
        char peek_str[5] = "peek";
        int peek_flag = 1;

        for (int i = 0; i < 4; i++) {
            if (peek_str[i] == curr_command[i]) continue;
            else {
                peek_flag = 0;
                break;
            }
        }

        // checking if peek command is present
        if (peek_flag) {
            char** argument_tokens = generate_tokens(curr_command, ' ');
            int no_of_arguments = 0;
            while(argument_tokens[no_of_arguments] != NULL) {
                no_of_arguments++;
            }
            no_of_arguments--;

            char path[MAX_LEN];
            for (int i = 0; i < strlen(cwd); i++) {
                path[i] = cwd[i];
            }
            path[strlen(cwd)] = '\0';

            int a = 0;
            int l = 0;

            if (no_of_arguments != 0) {
                char minus_a[3] = "-a";
                char minus_l[3] = "-l";
                char minus_al[4] = "-al";
                char minus_la[4] = "-la";

                int path_flag = 0;
                for (int i = 1; i <= no_of_arguments; i++) {
                    if (strcmp(argument_tokens[i], minus_a) == 0) {
                        a = 1;
                    } else if (strcmp(argument_tokens[i], minus_l) == 0) {
                        l = 1;
                    } else if (strcmp(argument_tokens[i], minus_la) == 0 || strcmp(argument_tokens[i], minus_al) == 0) {
                        a = 1;
                        l = 1;
                    } else {
                        path_flag = 1;
                        for (int j = 0; j < strlen(argument_tokens[i]); j++) {
                            path[j] = argument_tokens[i][j];
                        }
                        path[strlen(argument_tokens[i])] = '\0';
                    }
                }
            }
            int exit_code = peek(path, a, l, cwd, home_directory, prev_dir);
            // if (exit_code == 1 && store == 1) store_command(curr_command);
            if (exit_code == 0) overall_success = 0;
            free_tokens(argument_tokens);
        }
// ===================================================================================
        // pastevents
        char pastevents_str[11] = "pastevents";
        int pastevents_flag = 1;

        for (int i = 0; i < 10; i++) {
            if (pastevents_str[i] == curr_command[i]) continue;
            else {
                pastevents_flag = 0;
                break;
            }
        }

        // checking if pastevents command is present
        if (pastevents_flag) {
            pastevents_present = 1;
            char** argument_tokens = generate_tokens(curr_command, ' ');
            int no_of_arguments = 0;
            while(argument_tokens[no_of_arguments] != NULL) {
                no_of_arguments++;
            }
            no_of_arguments--;

            if (no_of_arguments == 0) {
                pastevents();
            } else {
                char purge_str[6] = "purge";
                char execute_str[8] = "execute";
                if (strcmp(argument_tokens[1], purge_str) == 0) {
                    purge();
                } else if (strcmp(argument_tokens[1], execute_str) == 0) {
                    if (no_of_arguments == 1) {
                        printf("pastevents: missing argument in \"%s\"\n", curr_command);
                    } else if (no_of_arguments == 2) {
                        char* number = argument_tokens[2];
                        int num = 0;
                        int flag = 1;
                        if (strcmp(number, "1") == 0) {
                            num = 1;
                        } else if (strcmp(number, "2") == 0) {
                            num = 2;
                        } else if (strcmp(number, "3") == 0) {
                            num = 3;
                        } else if (strcmp(number, "4") == 0) {
                            num = 4;
                        } else if (strcmp(number, "5") == 0) {
                            num = 5;
                        } else if (strcmp(number, "6") == 0) {
                            num = 6;
                        } else if (strcmp(number, "7") == 0) {
                            num = 7;
                        } else if (strcmp(number, "8") == 0) {
                            num = 8;
                        } else if (strcmp(number, "9") == 0) {
                            num = 9;
                        } else if (strcmp(number, "10") == 0) {
                            num = 10;
                        } else if (strcmp(number, "11") == 0) {
                            num = 11;
                        } else if (strcmp(number, "12") == 0) {
                            num = 12;
                        } else if (strcmp(number, "13") == 0) {
                            num = 13;
                        } else if (strcmp(number, "14") == 0) {
                            num = 14;
                        } else if (strcmp(number, "15") == 0) {
                            num = 15;
                        } else {
                            flag = 0;
                            printf("pastevents: argument value should a positive integer less than or equal to 15\n");
                        }
                        if (flag) {
                            int exit_code = execute(num, home_directory, cwd, prev_dir, store);
                        }
                    } else {
                        printf("pastevents: excess arguments in \"%s\"\n", curr_command);
                    }
                } else {
                    printf("pastevents: invalid arguments in \"%s\"\n", curr_command);
                }
            }
        }
// ===================================================================================
        idx++;
        curr_command = list_of_commands[idx];
    }
    if (store == 1 && pastevents_present == 0 && overall_success == 1) store_command(input);
    free(input);
    free_commands_list(list_of_commands);
}