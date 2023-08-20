#ifndef __SEEK_FILE_H
#define __SEEK_FILE_H

// int seek(char* directory, int d, int f, int e, char* file_name, char* path_of_last_file, int* no_of_files_dir_found);
char** seek(char* path_to_base_dir, char* file_name, int* no_of_files_or_dir_found, char** paths, int* size);
char** store_path(char* path_to_store, char** paths_array, int* size_of_paths_array, int* no_of_dir_or_files_found);

#endif