#include "headers.h"

int system_command(char** argument_tokens, int no_of_arguments, char* curr_command, int w, int ap, int ip, char* input_file_name_redirection, char* output_file_name_redirection) {
    if (strcmp(argument_tokens[0], "echo") == 0) {
        for (int i = 1; i <= no_of_arguments; i++) {
            if (argument_tokens[i] != NULL) {
                if (argument_tokens[i][0] == '"') {
                    for (int j = 0; j < strlen(argument_tokens[i]) - 1; j++) {
                        argument_tokens[i][j] = argument_tokens[i][j + 1];
                    }
                    argument_tokens[i][strlen(argument_tokens[i]) - 1] = '\0';
                } 
                if (argument_tokens[i][strlen(argument_tokens[i]) - 1] == '"') {
                    argument_tokens[i][strlen(argument_tokens[i]) - 1] = '\0';
                }
            } else {
                break;
            }
        }
    }

    bg_process = 0;

    if (curr_command[strlen(curr_command) - 1] == '&') {
        bg_process = 1;
        argument_tokens[no_of_arguments][strlen(argument_tokens[no_of_arguments]) - 1] = '\0';
    } else {
        bg_process = 0;
    }

    int ppid = getpid();
    int pid = fork();

    if (pid == 0) {
        if (bg_process == 1)  {
            setpgid(0, 0);
        }
        // creating absolute path to the file (input)
        char inp_file_path[MAX_LEN];
        if (input_file_name_redirection == NULL) {
            // do nothing
        } else if (input_file_name_redirection[0] != '/') {
            strcpy(inp_file_path, cwd);
            strcat(inp_file_path, "/");
            strcat(inp_file_path, input_file_name_redirection);
        } else if (input_file_name_redirection[0] == '/') {
            strcpy(inp_file_path, input_file_name_redirection);
        }

        // creating absolute path to the file (output)
        char out_file_path[MAX_LEN];
        if (output_file_name_redirection == NULL) {
            // do nothing
        } else if (output_file_name_redirection[0] != '/') {
            strcpy(out_file_path, cwd);
            strcat(out_file_path, "/");
            strcat(out_file_path, output_file_name_redirection);
        } else if (output_file_name_redirection[0] == '/') {
            strcpy(out_file_path, output_file_name_redirection);
        }

        if (w == 1 && ip == 1) {
            close(STDOUT_FILENO);
            open(out_file_path, O_CREAT | O_WRONLY | O_TRUNC, 0644);

            int inp_fd = open(inp_file_path, O_RDONLY);
            if (inp_fd < 0) {
                // open failed
                fprintf(stderr, "\033[1;31mopen : %s\033[1;0m\n", strerror(errno));
                // killing child process
                kill(getpid(), SIGTERM);
            } else {
                if (dup2(inp_fd, STDIN_FILENO) == -1) {
                    // dup2 failed
                    fprintf(stderr, "\033[1;31mdup2 : %s\033[1;0m\n", strerror(errno));
                    // closing the opened file
                    close(inp_fd);
                    kill(getpid(), SIGTERM);
                } else {
                    close(inp_fd);
                    execvp(argument_tokens[0],  argument_tokens);
                    // execvp failed
                    fprintf(stderr, "\033[1;31mexecvp : %s\033[1;0m\n", strerror(errno));
                    kill(getpid(), SIGTERM);
                }
            }
        } if (w == 1) {
            close(STDOUT_FILENO);
            open(out_file_path, O_CREAT | O_WRONLY | O_TRUNC, 0644);
            execvp(argument_tokens[0], argument_tokens);
            // error
            fprintf(stderr, "\033[1;31m%s : %s\033[1;0m\n", argument_tokens[0], strerror(errno));
            kill(getpid(), SIGTERM);
        } else if (ap == 1) {
            close(STDOUT_FILENO);
            open(out_file_path, O_CREAT | O_WRONLY | O_APPEND, 0644);
            execvp(argument_tokens[0], argument_tokens);
            // error
            fprintf(stderr, "\033[1;31m%s : %s\033[1;0m\n", argument_tokens[0], strerror(errno));
            kill(getpid(), SIGTERM);
        } else if (ip == 1) {
            int inp_fd = open(inp_file_path, O_RDONLY);
            if (inp_fd < 0) {
                // open failed
                fprintf(stderr, "\033[1;31mopen : %s\033[1;0m\n", strerror(errno));
                kill(getpid(), SIGTERM);
            } else {
                if (dup2(inp_fd, STDIN_FILENO) == -1) {
                    // dup2 failed
                    fprintf(stderr, "\033[1;31mdup2 : %s\033[1;0m\n", strerror(errno));
                    close(inp_fd);
                    kill(getpid(), SIGTERM);
                } else {
                    close(inp_fd);
                    execvp(argument_tokens[0],  argument_tokens);
                    // execvp failed
                    fprintf(stderr, "\033[1;31mexecvp : %s\033[1;0m\n", strerror(errno));
                    kill(getpid(), SIGTERM);
                }
            }
        } else {
            execvp(argument_tokens[0], argument_tokens);
            // execvp failed
            fprintf(stderr, "\033[1;31m%s : %s\033[1;0m\n", argument_tokens[0], strerror(errno));
            kill(getpid(), SIGTERM);
        }
    } else if (pid > 0) {
        if (bg_process == 0) {
            global_fg_pid = pid;

            char* temp = (char*) calloc(MAX_LEN, sizeof(char));
            strcpy(temp, curr_command);
            fg_command_name = temp;

            int cstatus;
            waitpid(pid, &cstatus, WUNTRACED);

            global_fg_pid = -1;
            free(fg_command_name);
            fg_command_name = NULL;
        } else {
            bg_gpid = pid;
            printf("%d\n", pid);
            insert_in_LL(pid, -1, argument_tokens);
        }
    } else {
        fprintf(stderr, "\033[1;31mfork: could not fork\033[1;0m\n");
        return 0;
    }
    return 1;
}