#include "headers.h"

int io_redirection(int ap, int w, char* cwd, char* file_name_redirection) {
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

            // opening file in append mode
            int fd = open(file_path, O_CREAT | O_WRONLY | O_APPEND, 0644);
            if (fd < 0) {
                // open fails
                fprintf(stderr, "\033[1;31mio_redirection : open: Error in opening file\033[1;0m\n");
                return 0;
            } else {
                if (write(fd, global_buffer, strlen(global_buffer)) < 0) {
                    // write fails
                    fprintf(stderr, "\033[1;31mio_redirection : write: Error in writing\033[1;0m\n");
                    close(fd);
                    return 0;
                }
                // clearing global buffer
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

            // opening file in write mode
            int fd = open(file_path, O_CREAT | O_WRONLY | O_TRUNC, 0644);
            if (fd < 0) {
                // open fail
                fprintf(stderr, "\033[1;31mio_redirection : open: Error in opening file\033[1;0m\n");
                return 0;
            } else {
                if (write(fd, global_buffer, strlen(global_buffer)) < 0) {
                    // write fail
                    fprintf(stderr, "\033[1;31mio_redirection : write: Error in writing\033[1;0m\n");
                    close(fd);
                    return 0;
                }
                // clearing global buffer
                memset(global_buffer, 0, strlen(global_buffer));
                close(fd);
            }
        }
    }
    return 1;
}

