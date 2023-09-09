#include "headers.h"

void seek(char* path_to_base_dir, char* file_name, linked_list_head paths) {
    struct stat dir_stat;
    stat(path_to_base_dir, &dir_stat);

    if (S_ISDIR(dir_stat.st_mode) == 0) {
        return;
    }

    DIR *dr;
    struct dirent *en;

    dr = opendir(path_to_base_dir);
    if (dr) {
        while ((en = readdir(dr)) != NULL) {
            if (en->d_name[0] != '.') {
                char* next_file_path = (char*) calloc(MAX_LEN, sizeof(char));
                strcpy(next_file_path, path_to_base_dir);
                update_path(next_file_path, en->d_name);

                seek(next_file_path, file_name, paths);

                char* new_file_name = remove_extension(file_name);
                char* curr_file_name = remove_extension(en->d_name);
                if (strcmp(new_file_name, curr_file_name) == 0) {
                    insert_in_linked_list(paths, next_file_path);
                }
                free(curr_file_name);
                free(next_file_path);
                free(new_file_name);
            }
        }
        closedir(dr);
    }
    return;
}

