#include "headers.h"

int warp(char* cwd, char* path, char* prev_dir, char* home_dir, int ap, int w) {
    if (strlen(path) > 0) {
        // checking if absolute path
        if (path[0] == '/') {
            struct stat path_stat;
            if (stat(path, &path_stat) < 0) {
                if (ap == 1 || w == 1) {
                    char buff[MAX_LEN] = {0};
                    sprintf(buff, "warp: no such directory exists: %s\n", path);
                    bprintf(global_buffer, buff);
                } else {
                    char buff[MAX_LEN] = {0};
                    sprintf(buff, "\033[1;31mwarp: no such directory exists: %s\033[1;0m\n", path);
                    bprintf(global_buffer, buff);  
                }
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
        if (ap == 0 || w == 0) {
            char buff[MAX_LEN] = {0};
            sprintf(buff, "warp: no such directory exists: %s\n", path);
            bprintf(global_buffer, buff);
        } else {
            char buff[MAX_LEN] = {0};
            sprintf(buff, "\033[1;31mwarp: no such directory exists: %s\033[1;0m\n", path);
            bprintf(global_buffer, buff);
        }
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
        // ============================================================================
        /*
            Earlier i was not using chdir() function and was keeping track of 
            cwd manually but then i realised that the system functions like ls etc. 
            when called from my terminal would run inside the system terminal and
            since it's pwd is not changed it would print results based on the file
            where my program resides so afterwards i have added just this one line
            to also change the pwd of the original terminal. I know after this i had
            no need to keep track of paths manually but since i had implemented this
            already i am not changing it. (So there may be places where i might have
            just coded based on my earlier implementation)
        */ 
        chdir(new_path);
        // ============================================================================

        free(new_path);

        char buff[MAX_LEN] = {0};
        sprintf(buff, "%s\n", cwd);
        bprintf(global_buffer, buff);
    }
    return 1;
}

