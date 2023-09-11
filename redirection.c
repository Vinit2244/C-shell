#include "headers.h"

void io_redirection(int ap, int w, char* cwd, char* file_name_redirection) {
    if (ap == 0 && w == 0) {
        print_global_buffer_onto_terminal();
    } else {
        if (ap == 1) {
            char file_path[MAX_LEN];

            if (file_name_redirection[0] != '/') {
                strcpy(file_path, cwd);
                strcat(file_path, "/");
                strcat(file_path, file_name_redirection);
            } else if (file_name_redirection[0] == '/') {
                strcpy(file_path, file_name_redirection);
            }

            int fd = open(file_path, O_CREAT | O_WRONLY | O_APPEND, 0644);
            if (fd < 0) {
                // printf("hi\n");
                printf("\033[1;31mopen: Error in opening file\033[1;0m\n");
                return;
            } else {
                if (write(fd, global_buffer, strlen(global_buffer)) < 0) {
                    printf("\033[1;31mwrite: Error in writing\033[1;0m\n");
                    close(fd);
                    return;
                }
                memset(global_buffer, 0, strlen(global_buffer));
                close(fd);
            }
        } else if (w == 1) {
            char file_path[MAX_LEN];

            if (file_name_redirection[0] != '/') {
                strcpy(file_path, cwd);
                strcat(file_path, "/");
                strcat(file_path, file_name_redirection);
            } else if (file_name_redirection[0] == '/') {
                strcpy(file_path, file_name_redirection);
            }

            int fd = open(file_path, O_CREAT | O_WRONLY | O_TRUNC, 0644);
            if (fd < 0) {
                printf("\033[1;31mopen: Error in opening file\033[1;0m\n");
                return;
            } else {
                if (write(fd, global_buffer, strlen(global_buffer)) < 0) {
                    printf("\033[1;31mwrite: Error in writing\033[1;0m\n");
                    close(fd);
                    return;
                }
                memset(global_buffer, 0, strlen(global_buffer));
                close(fd);
            }
        }
    }
}

