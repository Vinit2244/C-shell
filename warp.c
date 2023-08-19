#include "headers.h"

int warp(char* cwd, char* path, char* prev_dir, char* home_dir) {

    if (strlen(path) > 0) {
        // checking if absolute path
        if (path[0] == '/') {
            struct stat path_stat;
            if (stat(path, &path_stat) < 0) {
                printf("warp: no such directory exists: %s\n", path);
                return 0;
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
        return 0;
    } else if (new_path[0] == '^') {
        // print nothing
        free(new_path);
        return 0;
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
    return 1;
}
