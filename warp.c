#include "headers.h"

void warp(char* cwd, char* path, char* prev_dir, char* home_dir) {

    if (strlen(path) > 0) {
        // checking if absolute path
        if (path[0] == '/') {
            struct stat path_stat;
            if (stat(path, &path_stat) < 0) {
                printf("warp: no such directory exists: %s\n", path);
                return;
            } else {
                for (int i = 0; i < strlen(path); i++) {
                    cwd[i] = path[i];
                }
                cwd[strlen(path)] = '\0';
            }
        }
    }

    char* new_path = generate_new_path(cwd, path, prev_dir, home_dir);
    if (new_path == NULL) {
        printf("warp: no such directory exists: %s\n", path);
    } else {
        // change prev to cwd
        for (int i = 0; i < strlen(cwd); i++) {
            prev_dir[i] = cwd[i];
        }
        prev_dir[strlen(cwd)] = '\0';

        // change cwd to new path
        for (int i = 0; i < strlen(new_path); i++) {
            cwd[i] = new_path[i];
        }
        cwd[strlen(new_path)] = '\0';
        free(new_path);

        printf("%s\n", cwd);
    }
}

int check_if_such_dir_exists(char* path, char* next_dir) {
    DIR *dr;
    struct dirent *en;
    dr = opendir(path);

    if (dr) {
        while ((en = readdir(dr)) != NULL) {
            if (strcmp(en->d_name, next_dir) == 0) {
                if (check_if_dir(path, en->d_name) == 1) {
                    closedir(dr);
                    return 1;
                }
            }
        }
        closedir(dr);
        return 0;
    }
    return 0;
}

void update_path(char* path, char* next_dir) {
    int start = strlen(path);
    path[start] = '/';
    for (int i = start + 1; i < start + 1 + strlen(next_dir); i++) {
        path[i] = next_dir[i - start - 1];
    }
    path[start + 1 + strlen(next_dir)] = '\0';
}

int check_if_dir(char* path, char* file_name) {
    char complete_path[MAX_LEN];
    for (int i = 0; i < strlen(path); i++) {
        complete_path[i] = path[i];
    }
    complete_path[strlen(path)] = '/';
    for (int i = strlen(path) + 1; i < strlen(path) + 1 + strlen(file_name); i++) {
        complete_path[i] = file_name[i - strlen(path) - 1];
    }
    complete_path[strlen(path) + 1 + strlen(file_name)] = '\0';

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