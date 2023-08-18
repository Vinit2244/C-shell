#include "headers.h"

void wrap(char* cwd, char* path, char* prev_dir, char* home_dir) {
    char* tilde = "~";
    char* dash = "-";
    char* double_dots = "..";
    char* dot = ".";
    char* base_folder = "/";

    struct stat path_stat;
    stat(path, &path_stat);
    int a = S_ISDIR(path_stat.st_mode);
    if (a == 1) {
        // change prev to cwd
        for (int i = 0; i < strlen(cwd); i++) {
            prev_dir[i] = cwd[i];
        }
        prev_dir[strlen(cwd)] = '\0';

        // change cwd to path
        for (int i = 0; i < strlen(path); i++) {
            cwd[i] = path[i];
        }
        cwd[strlen(path)] = '\0';
        printf("%s\n", path);
        return;
    }

    char present_dir_temp[MAX_LEN];
    for (int i = 0; i < strlen(cwd); i++) {
        present_dir_temp[i] = cwd[i];
    }
    present_dir_temp[strlen(cwd)] = '\0';

    int success = 1;
    const char dir_delim[] = "/";

    char** dir_tokens = generate_tokens(path, '/');
    int dir_token_id = 0;

    while(dir_tokens[dir_token_id] != NULL) {
        int dir_exists = 0;
        char* next_dir = dir_tokens[dir_token_id];
        if (strcmp(next_dir, dot) == 0) {
            // do nothing
        } else if (strcmp(next_dir, double_dots) == 0) {
            if (strcmp(base_folder, cwd) == 0) {
                // do nothing as we cannot move to the parent of the base folder
            } else {
                int rear_index = strlen(cwd) - 1;
                while (cwd[rear_index] != '/') {
                    rear_index--;
                }
                cwd[rear_index] = '\0';
                if (strlen(cwd) == 0) {
                    cwd[0] = '/';
                    cwd[1] = '\0';
                }
            }
        } else if (strcmp(next_dir, tilde) == 0) {
            // change cwd to home_directory
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
                printf("wrap: no such directory exists: %s\n", path);
                success = 0;
                dir_token_id++;
                break;
            }
        }
        dir_token_id++;
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
    free_tokens(dir_tokens);
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

char** generate_tokens(char* str, char c) {
    int no_of_characters = 0;
    int idx = 0;
    while (str[idx] != '\0') {
        if (str[idx] == c) no_of_characters++;
        idx++;
    }
    int no_of_partitions = no_of_characters + 1;
    char** tokens_array = (char**) calloc(no_of_partitions + 1, sizeof(char*));
    for (int i = 0; i < no_of_partitions; i++) {
        tokens_array[i] = (char*) calloc(MAX_LEN, sizeof(char));
    }
    tokens_array[no_of_partitions] = NULL;
    int str_idx = 0;
    int tokens_array_idx = 0;
    int token_idx = 0;
    while (str[str_idx] != '\0') {
        if (str[str_idx] == c) {
            tokens_array[tokens_array_idx][token_idx] = '\0';
            token_idx = 0;
            tokens_array_idx++;
        } else {
            tokens_array[tokens_array_idx][token_idx] = str[str_idx];
            token_idx++;
        }
        str_idx++;
    }

    tokens_array[tokens_array_idx][token_idx] = '\0';
    return tokens_array;
}

void free_tokens(char** tokens) {
    int idx = 0;
    while (tokens[idx] != NULL) {
        free(tokens[idx]);
        idx++;
    }
    free(tokens);
}