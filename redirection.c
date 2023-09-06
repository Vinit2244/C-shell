#include "headers.h"

void io_redirection(int ap, int w, char* cwd, char* file_name_redirection) {
    if (ap == 0 && w == 0) {
        print_global_buffer_onto_terminal();
    } else {
        if (ap == 1) {
            char file_path[MAX_LEN];
            strcpy(file_path, cwd);
            strcat(file_path, "/");
            strcat(file_path, file_name_redirection);
            int fd = open(file_path, O_CREAT | O_WRONLY | O_APPEND, 0644);
            if (fd < 0) {
                printf("\033[1;31mopen: Error in opening file\033[1;0m\n");
            } else {
                if (write(fd, global_buffer, strlen(global_buffer)) < 0) {
                    printf("\033[1;31mwrite: Error in writing\033[1;0m\n");
                }
                memset(global_buffer, 0, strlen(global_buffer));
                close(fd);
            }
        } else if (w == 1) {
            char file_path[MAX_LEN];
            strcpy(file_path, cwd);
            strcat(file_path, "/");
            strcat(file_path, file_name_redirection);
            int fd = open(file_path, O_CREAT | O_WRONLY | O_TRUNC, 0644);
            if (fd < 0) {
                printf("\033[1;31mopen: Error in opening file\033[1;0m\n");
            } else {
                if (write(fd, global_buffer, strlen(global_buffer)) < 0) {
                    printf("\033[1;31mwrite: Error in writing\033[1;0m\n");
                }
                memset(global_buffer, 0, strlen(global_buffer));
                close(fd);
            }
        }
    }
}

// int write_redirect(char** params) {
//     char* from_file = params[0];
//     char* to_file = params[1];

//     int rc = fork();
//     if (rc < 0) {
//         printf("fork: fork failed\n");
//         return 0;
//     } else if (rc == 0) {
//         close(STDOUT_FILENO);
//         open(to_file, O_CREAT | O_WRONLY | O_TRUNC, 0644);

//         char* myargs[3];
//         myargs[0] = strdup(from_file);
//         myargs[1] = strdup(to_file);
//         myargs[2] = NULL;
//         execvp(myargs[0], myargs);
//     } else {
//         int wc = wait(NULL);
//         return 1;
//     }
//     return 1;
// }