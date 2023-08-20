#include "headers.h"

char** seek(char* path_to_base_dir, char* file_name, int* no_of_files_or_dir_found, char** paths, int* size) {
    printf("1");
    DIR *dr;
    struct dirent *en;
    struct stat dir_stat;
    stat(path_to_base_dir, &dir_stat);

    if (!S_ISDIR(dir_stat.st_mode)) {
        return paths;
    }

    dr = opendir(path_to_base_dir);
    if (dr) {
        while ((en = readdir(dr)) != NULL) {
            char* next_file_path = (char*) calloc(MAX_LEN, sizeof(char));

            strcpy(next_file_path, path_to_base_dir);
            update_path(next_file_path, en->d_name);
            paths = seek(next_file_path, file_name, no_of_files_or_dir_found, paths, size);

            char* curr_file_name = remove_extension(en->d_name);
            if (strcmp(file_name, curr_file_name) == 0) {
                paths = store_path(next_file_path, paths, size, no_of_files_or_dir_found);
                *no_of_files_or_dir_found += 1;
            }
            free(curr_file_name);
            free(next_file_path);
        }
        closedir(dr);
    }
    return paths;
}

char** store_path(char* path_to_store, char** paths_array, int* size_of_paths_array, int* no_of_dir_or_files_found) {
    if (no_of_dir_or_files_found == size_of_paths_array) {
        // realloc the memory
        char** new_paths_array = (char**) calloc(((*size_of_paths_array) * 2) + 1, sizeof(char*));
        for (int i = 0; i < (*size_of_paths_array); i++) {
            new_paths_array[i] = paths_array[i];
        }
        for (int j = (*size_of_paths_array); j < ((*size_of_paths_array) * 2); j++) {
            new_paths_array[j] = (char*) calloc(MAX_LEN, sizeof(char));
        }
        new_paths_array[(*size_of_paths_array) * 2] = NULL;
        free_tokens(paths_array);

        for (int i = 0; i < strlen(path_to_store); i++) {
            new_paths_array[*(no_of_dir_or_files_found)][i] = path_to_store[i];
        }
        new_paths_array[(*no_of_dir_or_files_found)][strlen(path_to_store)] = '\0';
        *no_of_dir_or_files_found += 1;
        *size_of_paths_array = ((*size_of_paths_array) * 2);
        return new_paths_array;
    } else {
        for (int i = 0; i < strlen(path_to_store); i++) {
            paths_array[*no_of_dir_or_files_found][i] = path_to_store[i];
        }
        paths_array[*no_of_dir_or_files_found][strlen(path_to_store)] = '\0';
        *no_of_dir_or_files_found += 1;
        return paths_array;
    }
}