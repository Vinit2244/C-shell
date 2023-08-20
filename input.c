#include "headers.h"

void input(char* command, char* home_directory, char* cwd, char* prev_dir, int store, char* last_command, int* t) {
    // handling signals from child
    struct sigaction sa;
    
    sa.sa_sigaction = handle_child_signal;
    sa.sa_flags = SA_SIGINFO;
    
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
    }
    
    int overall_success = 1;
    int pastevents_present = 0;
    char* input = (char*) calloc(5000, sizeof(char));
    if (command == NULL) {
        store = 1;
        // Print appropriate prompt with username, systemname and directory before accepting input
        prompt(home_directory, cwd, t, last_command);
        
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
        read_and_free_LL(); // printing the status of the background processes if any completed

        // ================= Printing the commands =================
        // printf("Command %d: %s\n", idx + 1, curr_command);

        char** argument_tokens = generate_tokens(curr_command, ' ');
        int no_of_arguments = 0;
        while(argument_tokens[no_of_arguments] != NULL) {
            no_of_arguments++;
        }
        no_of_arguments--;

        // Different commands
// ===================================================================================
        // warp
        // char warp_str[5] = "warp";
        // int warp_flag = check(warp_str, curr_command, 4);

        // checking if warp command is present
        if (strcmp("warp", argument_tokens[0]) == 0) {
            int success = 1;
            if (no_of_arguments == 0) {
                char* c = "~";
                int exit_code = warp(cwd, c, prev_dir, home_directory);
                if (exit_code == 0) success = 0;
            } else {
                for (int i = 1; i <= no_of_arguments; i++) {
                    int exit_code = warp(cwd, argument_tokens[i], prev_dir, home_directory);
                    if (exit_code == 0) success = 0;
                }
            }
            if (success == 0) overall_success = 0;
            // goto next_iteration;
        }
// ===================================================================================
        // peek
        // char peek_str[5] = "peek";
        // int peek_flag = check(peek_str, curr_command, 4);

        // checking if peek command is present
        else if (strcmp("peek", argument_tokens[0]) == 0) {
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
            if (exit_code == 0) overall_success = 0;
            // goto next_iteration;
        }
// ===================================================================================
        // pastevents
        // char pastevents_str[11] = "pastevents";
        // int pastevents_flag = check(pastevents_str, curr_command, 10);

        // checking if pastevents command is present
        else if (strcmp("pastevents", argument_tokens[0]) == 0) {
            pastevents_present = 1;
            if (no_of_arguments == 0) {
                pastevents();
            } else {
                if (strcmp(argument_tokens[1], "purge") == 0) {
                    purge();
                } else if (strcmp(argument_tokens[1], "execute") == 0) {
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
                            printf("%d\n", num);
                            int exit_code = execute(num, home_directory, cwd, prev_dir, store, last_command, t);
                        }
                    } else {
                        printf("pastevents: excess arguments in \"%s\"\n", curr_command);
                    }
                } else {
                    printf("pastevents: invalid arguments in \"%s\"\n", curr_command);
                }
            }
            // goto next_iteration;
        }
// ===================================================================================
        // // proclore
        // char proclore_str[9] = "proclore";
        // int proclore_flag = check(proclore_str, curr_command, 8);

        // // checking if proclore command is present
        // if (proclore_flag) {
        //     if (no_of_arguments == 0) {

        //     } else if (no_of_arguments == 1) {
        //         int pid = str_to_int(argument_tokens[1]);
        //         if (pid == -1) {
        //             printf("proclore: invalid argument (argument should be number)\n");
        //         } else {
        //             printf("pid : %d\n", pid);

        //         }
        //     } else {
        //         printf("proclore: excess number of arguments\n");
        //     }
        //     goto next_iteration;
        // }
// ===================================================================================
        // // seek
        // char seek_str[5] = "seek";
        // int seek_flag = check(seek_str, curr_command, 4);

        // // checking if seek command is present
        // if (seek_flag) {
        //     char d_flag[3] = "-d";
        //     char f_flag[3] = "-f";
        //     char e_flag[3] = "-e";

        //     char* base_dir = (char*) calloc(MAX_LEN, sizeof(char));
        //     char* file_name = (char*) calloc(MAX_LEN, sizeof(char));

        //     int d = 0;
        //     int f = 0;
        //     int e = 0;

        //     int base_dir_flag = 0;
        //     int valid_flags = 1;
        //     int file_name_flag = 0;

        //     int success = 1;

        //     if (no_of_arguments == 0) {
        //         printf("seek: missing arguments\n");
        //         overall_success = 0;
        //         goto free_mem;
        //     } else {
        //         for (int i = 1; i <= no_of_arguments; i++) {
        //             char* curr_argument = argument_tokens[i];

        //             if (curr_argument[0] == '-') {
        //                 if (base_dir_flag == 1) {
        //                     printf("seek: Invalid Arguments\n");
        //                     overall_success = 0;
        //                     goto free_mem;
        //                 } else {
        //                     if (strcmp(curr_argument, d_flag) == 0) {
        //                         d = 1;
        //                     } else if (strcmp(curr_argument, f_flag) == 0) {
        //                         f = 1;
        //                     } else if (strcmp(curr_argument, e_flag) == 0) {
        //                         e = 1;
        //                     } else {
        //                         printf("seek: Invalid Flag\n");
        //                         overall_success = 0;
        //                         valid_flags = 0;
        //                         goto free_mem;
        //                     }
        //                 }
        //             } else if (curr_argument[0] == '.') {
        //                 base_dir_flag = 1;
        //                 strcpy(base_dir, curr_argument);
        //             } else {
        //                 if (file_name_flag == 1) {
        //                     printf("seek: invalid arguments\n");
        //                     overall_success = 0;
        //                     goto free_mem;
        //                 }
        //                 file_name_flag = 1;
        //                 for (int j = 0; j < strlen(curr_argument); j++) {
        //                     file_name[j] = curr_argument[j];
        //                 }
        //                 file_name[strlen(curr_argument)] = '\0';
        //             }
        //         }
        //     }
        //     if (valid_flags == 1 && file_name_flag == 1) {
        //         if (d == 1 && f == 1) {
        //             printf("Invalid flags\n");
        //             overall_success = 0;
        //             goto free_mem;
        //         } else {
        //             int size = 20;
        //             int no_of_files_or_dir_found = 0;
        //             char** paths;
        //             paths = (char**) calloc(size + 1, sizeof(char*));
        //             for (int i = 0; i < size; i++) {
        //                 paths[i] = (char*) calloc(MAX_LEN, sizeof(char));
        //             }
        //             paths[size] = NULL;

        //             // printf("%s %s\n", base_dir, file_name);
                    
        //             char* path_to_base_dir;
        //             if (base_dir_flag) {
        //                 path_to_base_dir = generate_new_path(cwd, base_dir, prev_dir, home_directory);
        //             } else {
        //                 path_to_base_dir = (char*) calloc(MAX_LEN, sizeof(char));
        //                 strcpy(path_to_base_dir, cwd);
        //             }
        //             printf("%s\n", path_to_base_dir);
        //             printf("%s\n", file_name);
        //             printf("%p\n", &no_of_files_or_dir_found);
        //             printf("%p\n", &size);
        //             paths = seek(path_to_base_dir, file_name, &no_of_files_or_dir_found, paths, &size);

        //             printf("%d\n", no_of_files_or_dir_found);
        //             free(path_to_base_dir);
        //             free_tokens(paths);
        //         }
        //     }

        //     free(base_dir);
        //     free(file_name);
        //     if (success == 0) overall_success = 0;
        //     goto next_iteration;
        // }
// ===================================================================================
        // exit
        // char exit_str[5] = "exit";
        // int exit_flag = check(exit_str, curr_command, 4);

        // checking if exit command is present
        else if (strcmp("exit", argument_tokens[0]) == 0) {
            if (no_of_arguments > 1) {
                printf("Invalid command\n");
            } else {
                store_command(curr_command);
                exit(0);
            }
        }
// ===================================================================================
        // echo
        // char echo_str[5] = "echo";
        // int echo_flag = check(echo_str, curr_command, 4);

        // checking if echo command is present
        else if (strcmp("echo", argument_tokens[0]) == 0) {
            for (int i = 1; i <= no_of_arguments; i++) {
                for (int j = 0; j < strlen(argument_tokens[no_of_arguments]); j++) {
                    if (argument_tokens[i][j] == '"') continue;
                    printf("%c", argument_tokens[i][j]);
                }
                printf(" ");
            }
            printf("\n");
            // goto next_iteration;
        }
// ===================================================================================
        else {
            // system commands
            int bg_process = 0;
            if (curr_command[strlen(curr_command) - 1] == '&') {
                bg_process = 1;
                argument_tokens[no_of_arguments][strlen(argument_tokens[no_of_arguments]) - 1] = '\0';
            }

            int start = 0;
            time_t tyme = 0;
            int status;
            // int ppid = getpid();
            int pid = fork();
            if (pid == 0) {

                execvp(argument_tokens[0], argument_tokens);
                perror(argument_tokens[0]);
            } else if (pid > 0) {
                if (!bg_process) {
                    start = time(NULL);
                    wait(NULL);
                    tyme = time(NULL) - start;

                    *t = tyme;
                    strcpy(last_command, argument_tokens[0]);
                } else {
                    printf("%d\n", pid);
                    // char* str = (char*) calloc(200, sizeof(char));
                    // strcpy(str, "background process ended normally (");
                    // char* num = (char*) calloc(10, sizeof(char));
                    // sprintf(num, "%d", getpid());
                    // strcat(str, num);
                    // free(num);
                    // strcat(str, ")");
                    // insert_in_LL(LL, pid);
                }
            } else {
                perror("fork");
            }
        }
// ===================================================================================
    // next_iteration:
        free_tokens(argument_tokens);
        idx++;
        curr_command = list_of_commands[idx];
    }
    if (store == 1 && pastevents_present == 0 && overall_success == 1) store_command(input);
    free(input);
    free_commands_list(list_of_commands);
}

// int check(char* str, char* command, int len) {
//     for (int i = 0; i < len; i++) {
//         if (str[i] == command[i]) continue;
//         else return 0;
//     }
//     return 1;
// }

void handle_child_signal(int signum, siginfo_t *info, void *context) {
    // printf("Received signal %d from process with PID %d.\n", signum, info->si_pid);
    // Additional handling logic can go here
    if (signum == 20) {
        insert_in_LL(info->si_pid, 1);
    } else {
        insert_in_LL(info->si_pid, 0);
    }
}