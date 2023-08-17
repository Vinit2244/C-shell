#include "headers.h"

void wrap(char* path, char* cwd, char* prev_dir, char* home_dir) {
    char* tilde = "~";
    char* dash = "-";
    char* double_dots = "..";
    char* dot = ".";

    const char command_delim[] = " ";

    // First call to strtok with the input string
    char *separate_command_token = strtok(path, command_delim);

    // Subsequent calls with NULL to continue tokenizing
    while (separate_command_token != NULL) {
        char present_dir_temp[MAX_LEN];
        for (int i = 0; i < strlen(cwd); i++) {
            present_dir_temp[i] = cwd[i];
        }
        present_dir_temp[strlen(cwd)] = '\0';

        int success = 1;
        const char dir_delim[] = "/";
        char *dir_token = strtok(separate_command_token, dir_delim);
        while(dir_token != NULL) {
            int dir_exists = 0;
            char* next_dir = dir_token;
            if (strcmp(next_dir, dot) == 0) {
                // do nothing
            } else if (strcmp(next_dir, double_dots) == 0) {
                // write code for double dot (parent folder)
                int rear_index = strlen(cwd) - 1;
                while (cwd[rear_index] != '/') {
                    rear_index--;
                }
                cwd[rear_index] = '\0';
            } else if (strcmp(next_dir, tilde) == 0) {
                for (int i = 0; i < strlen(home_dir); i++) {
                    cwd[i] = home_dir[i];
                }
                cwd[strlen(home_dir)] = '\0';
            } else if (strcmp(next_dir, dash) == 0) {
                for (int i = 0; i < strlen(prev_dir); i++) {
                    cwd[i] = prev_dir[i];
                }
                cwd[strlen(prev_dir)] = '\0';
            } else {
                check_if_such_dir_exists(cwd, &dir_exists, next_dir);
                if (dir_exists == 1) {
                    update_cwd(cwd, next_dir);
                } else {
                    printf("wrap: no such directory exists: %s\n", separate_command_token);
                    success = 0;
                    dir_token = strtok(NULL, dir_delim);
                    break;
                }
            }
            dir_token = strtok(NULL, dir_delim);
        }
        if (success == 1) {
            // update the previous directory
            for (int i = 0; i < strlen(present_dir_temp); i++) {
                prev_dir[i] = present_dir_temp[i];
            }
            prev_dir[strlen(present_dir_temp)] = '\0';
            printf("%s\n", cwd);
        } else {
            // reset the current directory to previous directory
            for (int i = 0; i < strlen(present_dir_temp); i++) {
                cwd[i] = present_dir_temp[i];
            }
            cwd[strlen(present_dir_temp)] = '\0';
        }

        separate_command_token = strtok(NULL, command_delim);
    }
}

void check_if_such_dir_exists(char* cwd, int* dir_exists, char* next_dir) {
    DIR *dr;
    struct dirent *en;
    dr = opendir(cwd);

    if (dr) {
        while ((en = readdir(dr)) != NULL) {
            if (strcmp(en->d_name, next_dir) == 0) {
                if (check_if_dir(cwd, en->d_name) == 1) {
                    *dir_exists = 1;
                    break;
                }
            }
        }
        closedir(dr);
    }
}

void update_cwd(char* cwd, char* next_dir) {
    int start = strlen(cwd);
    cwd[start] = '/';
    for (int i = start + 1; i < start + 1 + strlen(next_dir); i++) {
        cwd[i] = next_dir[i - start - 1];
    }
    cwd[start + 1 + strlen(next_dir)] = '\0';
}

int check_if_dir(char* cwd, char* file_name) {
    char complete_path[MAX_LEN];
    for (int i = 0; i < strlen(cwd); i++) {
        complete_path[i] = cwd[i];
    }
    complete_path[strlen(cwd)] = '/';
    for (int i = strlen(cwd) + 1; i < strlen(cwd) + 1 + strlen(file_name); i++) {
        complete_path[i] = file_name[i - strlen(cwd) - 1];
    }
    complete_path[strlen(cwd) + 1 + strlen(file_name)] = '\0';

    struct stat path_stat;
    stat(complete_path, &path_stat);
    return S_ISDIR(path_stat.st_mode);
}