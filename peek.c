#include "headers.h"

void peek(char* path, int a, int l, char* cwd, char* home_dir, char* prev_dir) {
    char* new_path = (char*) calloc(MAX_LEN, sizeof(char));
    if (path[0] == '-') {
        printf("peek: no such file or directory\n");
        return;
    } else if (path[0] == '\0') {
        for (int i = 0; i < strlen(cwd); i++) {
            path[i] = cwd[i];
        }
        path[strlen(cwd)] = '\0';
    } else if (path[0] == '/') {
        for (int i = 0; i < strlen(path); i++) {
            new_path[i] = path[i];
        }
        new_path[strlen(path)] = '\0';
    } else {
        new_path = generate_new_path(cwd, path, prev_dir, home_dir);
    }

    char** files_list = generate_sorted_file_list(new_path);

    char complete_path_of_file[MAX_LEN];
    for (int i = 0; i < strlen(new_path); i++) {
        complete_path_of_file[i] = new_path[i];
    }
    complete_path_of_file[strlen(new_path)] = '/';
    int start = strlen(new_path) + 1;
    if (a && l) {
        int idx = 0;
        while (files_list[idx] != NULL) {
            for (int j = start; j < start + strlen(files_list[idx]); j++) {
                complete_path_of_file[j] = files_list[idx][j - start];
            }
            complete_path_of_file[start + strlen(files_list[idx])] = '\0';
            print_extra_details(complete_path_of_file, files_list, idx);
            idx++;
        }
    } else if (a) {
        int idx = 0;
        while (files_list[idx] != NULL) {
            for (int j = start; j < start + strlen(files_list[idx]); j++) {
                complete_path_of_file[j] = files_list[idx][j - start];
            }
            complete_path_of_file[start + strlen(files_list[idx])] = '\0';

            struct stat file_info;
            stat(complete_path_of_file, &file_info);
            if (S_ISDIR(file_info.st_mode)) { // checking if directory
                // color blue
                printf("\033[1;34m%s\033[1;0m\n", files_list[idx]);
            } else if (file_info.st_mode & S_IXUSR) { // checking if a file is executable
                // color green
                printf("\033[1;32m%s\033[1;0m\n", files_list[idx]);
            } else { // else the file is regular
                // color white
                printf("%s\n", files_list[idx]);
            }
            idx++;
        }
    } else if (l) {
        char complete_path_of_file[MAX_LEN];
        for (int i = 0; i < strlen(new_path); i++) {
            complete_path_of_file[i] = new_path[i];
        }
        complete_path_of_file[strlen(new_path)] = '/';
        int start = strlen(new_path) + 1;

        int idx = 0;
        while (files_list[idx] != NULL) {
            if (files_list[idx][0] == '.') {
                idx++;
                continue;
            } else {
                for (int j = start; j < start + strlen(files_list[idx]); j++) {
                complete_path_of_file[j] = files_list[idx][j - start];
                }
                complete_path_of_file[start + strlen(files_list[idx])] = '\0';
                print_extra_details(complete_path_of_file, files_list, idx);
            }
            idx++;
        }
    } else {
        int idx = 0;
        while (files_list[idx] != NULL) {
            if (files_list[idx][0] == '.') {
                idx++;
                continue;
            }

            for (int j = start; j < start + strlen(files_list[idx]); j++) {
                complete_path_of_file[j] = files_list[idx][j - start];
            }
            complete_path_of_file[start + strlen(files_list[idx])] = '\0';

            struct stat file_info;
            stat(complete_path_of_file, &file_info);
            if (S_ISDIR(file_info.st_mode)) { // checking if directory
                // color blue
                printf("\033[1;34m%s\033[1;0m\n", files_list[idx]);
            } else if (file_info.st_mode & S_IXUSR) { // checking if a file is executable
                // color green
                printf("\033[1;32m%s\033[1;0m\n", files_list[idx]);
            } else { // else the file is regular
                // color white
                printf("%s\n", files_list[idx]);
            }
            idx++;
        }
    }
    free(new_path);
    free_tokens(files_list);
}

char** generate_sorted_file_list(char* path) {
    DIR *dr;
    struct dirent *en;

    int no_of_files = 0;

    dr = opendir(path);
    if (dr) {
        while ((en = readdir(dr)) != NULL) {
            no_of_files++;
        }
        closedir(dr);
    }

    char** files_list = (char**) calloc(no_of_files + 1, sizeof(char*));
    for (int i = 0; i < no_of_files; i++) {
        files_list[i] = (char*) calloc(MAX_LEN, sizeof(char));
    }
    files_list[no_of_files] = NULL;

    DIR *dr1;
    struct dirent *en1;

    int file_no = 0;

    dr1 = opendir(path);

    if (dr1) {
        while ((en1 = readdir(dr1)) != NULL) {
            for (int i = 0; i < strlen(en1->d_name); i++) {
                files_list[file_no][i] = en1->d_name[i];
            }
            files_list[file_no][strlen(en1->d_name)] = '\0';
            file_no++;
        }
        closedir(dr1);
    }

    sort_strings(files_list, no_of_files);
    return files_list;
}

char* generate_new_path(char* cwd, char* path, char* prev_dir, char* home_dir) {
    int success = 1;

    char tilde[2]       = "~";
    char dash[2]        = "-";
    char double_dots[3] = "..";
    char dot[2]         = ".";
    char base_folder[2] = "/";

    char* new_path = (char*) calloc(MAX_LEN, sizeof(char));
    for (int i = 0; i < strlen(cwd); i++) {
        new_path[i] = cwd[i];
    }
    new_path[strlen(cwd)] = '\0';

    char** dir_tokens = generate_tokens(path, '/');
    int dir_tokens_idx = 0;

    while (dir_tokens[dir_tokens_idx] != NULL) {
        char* curr_token = dir_tokens[dir_tokens_idx];
        if (strcmp(curr_token, tilde) == 0) {
            // change cwd to home
            for (int i = 0; i < strlen(home_dir); i++) {
                new_path[i] = home_dir[i];
            }
            new_path[strlen(home_dir)] = '\0';
            // printf("%s\n", new_path);
        } else if (strcmp(curr_token, dash) == 0) {
            // change cwd to prev
            // checking if the warp - command is called just after the first initiation
            if (prev_dir[0] == '\0') {
                success = 2;
                printf("OLDPWD not set\n");
                // don't change the cwd
            } else {
                for (int i = 0; i < strlen(prev_dir); i++) {
                    new_path[i] = prev_dir[i];
                }
                new_path[strlen(prev_dir)] = '\0';
            }
        } else if (strcmp(curr_token, double_dots) == 0) {
            if (strcmp(base_folder, cwd) == 0) {
                // do nothing as we cannot move to the parent of the base folder
            } else {
                int rear_index = strlen(new_path) - 1;
                while (new_path[rear_index] != '/') {
                    rear_index--;
                }
                new_path[rear_index] = '\0';
                if (strlen(new_path) == 0) {
                    new_path[0] = '/';
                    new_path[1] = '\0';
                }
            }
        } else if (strcmp(curr_token, dot) == 0) {
            // do nothing
        } else {
            int dir_exists = check_if_such_dir_exists(new_path, curr_token);
            if (dir_exists == 1) {
                update_path(new_path, curr_token);
            } else {
                success = 0;
                break;
            }
        }
        dir_tokens_idx++;
    }
    free_tokens(dir_tokens);
    if (success == 1) return new_path;
    else if (success == 2) {
        char* temp = (char*) malloc(2 * sizeof(char));
        temp[0] = '^';
        temp[1] = '\0';
        return temp;
    } else {
        free(new_path);
        return NULL;
    }
}

void sort_strings(char** strings, int no_of_strings) {
    char s[MAX_LEN];
    for (int i = 0; i < no_of_strings; i++) {
        for(int j = i + 1; j < no_of_strings; j++) {
            if (strcmp(strings[i], strings[j]) > 0) {
            strcpy(s, strings[i]);
            strcpy(strings[i], strings[j]);
            strcpy(strings[j], s);
            }
        }
    }
}

void print_extra_details(char* complete_path_of_file, char** files_list, int idx) {
    DIR *dr;
    struct dirent *en;
    struct stat file_stat;

    if (!stat(complete_path_of_file, &file_stat)) {
        printf((S_ISDIR(file_stat.st_mode))  ? "d" : "-");
        printf((file_stat.st_mode & S_IRUSR) ? "r" : "-");
        printf((file_stat.st_mode & S_IWUSR) ? "w" : "-");
        printf((file_stat.st_mode & S_IXUSR) ? "x" : "-");
        printf((file_stat.st_mode & S_IRGRP) ? "r" : "-");
        printf((file_stat.st_mode & S_IWGRP) ? "w" : "-");
        printf((file_stat.st_mode & S_IXGRP) ? "x" : "-");
        printf((file_stat.st_mode & S_IROTH) ? "r" : "-");
        printf((file_stat.st_mode & S_IWOTH) ? "w" : "-");
        printf((file_stat.st_mode & S_IXOTH) ? "x" : "-");
    } else {
        perror("Error in stat");
    }

    struct passwd *pw = getpwuid(file_stat.st_uid);
    struct group  *gr = getgrgid(file_stat.st_gid);
    printf(" %d %s %s %lld ", file_stat.st_nlink, pw->pw_name, gr->gr_name, file_stat.st_size);

    time_t modificationTime = file_stat.st_mtime;

    // Convert time to a formatted string and print it
    char* time_string;
    time_string = ctime(&modificationTime);
    char month[4];
    char date[3];
    char hour[3];
    char mins[3];
    char year[5];
    for (int i = 4; i < 7; i++) {
        month[i - 4] = time_string[i];
    }
    month[3] = '\0';

    for (int i = 8; i < 10; i++) {
        date[i - 8] = time_string[i];
    }
    date[2] = '\0';

    for (int i = 11; i < 13; i++) {
        hour[i - 11] = time_string[i];
    }
    hour[2] = '\0';

    for (int i = 14; i < 16; i++) {
        mins[i - 14] = time_string[i];
    }
    mins[2] = '\0';

    for (int i = 20; i < 24; i++) {
        year[i - 20] = time_string[i];
    }
    year[4] = '\0';

    printf("%s %s %s %s ", month, date, hour, mins);

    if (S_ISDIR(file_stat.st_mode)) { // checking if directory
        // color blue
        printf("\033[1;34m%s\033[1;0m\n", files_list[idx]);
    } else if (file_stat.st_mode & S_IXUSR) { // checking if a file is executable
        // color green
        printf("\033[1;32m%s\033[1;0m\n", files_list[idx]);
    } else { // else the file is regular
        // color white
        printf("%s\n", files_list[idx]);
    }
}