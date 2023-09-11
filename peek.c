#include "headers.h"

int peek(char* path, int a, int l, char* cwd, char* home_dir, char* prev_dir, int ap, int w) {
    char* new_path = (char*) calloc(MAX_LEN, sizeof(char));
    if (path[0] == '-') {
        printf("\033[1;31mpeek: Invalid Flags\033[1;0m\n");
        return 0;
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
        if (new_path == NULL) {
            printf("\033[1;31mpeek: no such file or directory\033[1;0m\n");
            return 0;
        }
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
            int exit_status = print_extra_details(complete_path_of_file, files_list, idx, ap, w);
            if (exit_status == 0) return 0;
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
                if (ap == 0 && w == 0) {
                    char buff[MAX_LEN];
                    sprintf(buff, "\033[1;34m%s\033[1;0m\n", files_list[idx]);
                    bprintf(global_buffer, buff);
                } else {
                    char buff[MAX_LEN];
                    sprintf(buff, "%s\n", files_list[idx]);
                    bprintf(global_buffer, buff);
                }
            } else if (file_info.st_mode & S_IXUSR) { // checking if a file is executable
                // color green
                if (ap == 0 && w == 0) {
                    char buff[MAX_LEN];
                    sprintf(buff, "\033[1;32m%s\033[1;0m\n", files_list[idx]);
                    bprintf(global_buffer, buff);
                } else {
                    char buff[MAX_LEN];
                    sprintf(buff, "%s\n", files_list[idx]);
                    bprintf(global_buffer, buff);
                }
            } else { // else the file is regular
                // color white
                char buff[MAX_LEN];
                sprintf(buff, "%s\n", files_list[idx]);
                bprintf(global_buffer, buff);
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
                int exit_status = print_extra_details(complete_path_of_file, files_list, idx, ap, w);
                if (exit_status == 0) return 0;
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
                if (ap == 0 && w == 0) {
                    char buff[MAX_LEN];
                    sprintf(buff, "\033[1;34m%s\033[1;0m\n", files_list[idx]);
                    bprintf(global_buffer, buff);
                } else {
                    char buff[MAX_LEN];
                    sprintf(buff, "%s\n", files_list[idx]);
                    bprintf(global_buffer, buff);
                }
            } else if (file_info.st_mode & S_IXUSR) { // checking if a file is executable
                // color green
                if (ap == 0 && w == 0) {
                    char buff[MAX_LEN];
                    sprintf(buff, "\033[1;32m%s\033[1;0m\n", files_list[idx]);
                    bprintf(global_buffer, buff);
                } else {
                    char buff[MAX_LEN];
                    sprintf(buff, "%s\n", files_list[idx]);
                    bprintf(global_buffer, buff);
                }
            } else { // else the file is regular
                // color white
                char buff[MAX_LEN];
                sprintf(buff, "%s\n", files_list[idx]);
                bprintf(global_buffer, buff);
            }
            idx++;
        }
    }
    free(new_path);
    free_tokens(files_list);
    return 1;
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

int print_extra_details(char* complete_path_of_file, char** files_list, int idx, int ap, int w) {
    struct stat file_stat;

    if (!stat(complete_path_of_file, &file_stat)) {
        bprintf(global_buffer, (S_ISDIR(file_stat.st_mode))  ? "d" : "-");
        bprintf(global_buffer, (file_stat.st_mode & S_IRUSR) ? "r" : "-");
        bprintf(global_buffer, (file_stat.st_mode & S_IWUSR) ? "w" : "-");
        bprintf(global_buffer, (file_stat.st_mode & S_IXUSR) ? "x" : "-");
        bprintf(global_buffer, (file_stat.st_mode & S_IRGRP) ? "r" : "-");
        bprintf(global_buffer, (file_stat.st_mode & S_IWGRP) ? "w" : "-");
        bprintf(global_buffer, (file_stat.st_mode & S_IXGRP) ? "x" : "-");
        bprintf(global_buffer, (file_stat.st_mode & S_IROTH) ? "r" : "-");
        bprintf(global_buffer, (file_stat.st_mode & S_IWOTH) ? "w" : "-");
        bprintf(global_buffer, (file_stat.st_mode & S_IXOTH) ? "x" : "-");
    } else {
        printf("\033[1;31mpeek: error in stat\033[1;0m\n");
        return 0;
    }

    struct passwd *pw = getpwuid(file_stat.st_uid);
    struct group  *gr = getgrgid(file_stat.st_gid);

    char buff[MAX_LEN];
    sprintf(buff, " %hu %s %s %lld ", file_stat.st_nlink, pw->pw_name, gr->gr_name, file_stat.st_size);
    bprintf(global_buffer, buff);

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

    char buff2[MAX_LEN];
    sprintf(buff2, "%s %s %s %s ", month, date, hour, mins);
    bprintf(global_buffer, buff2);

    if (S_ISDIR(file_stat.st_mode)) { // checking if directory
        // color blue
        if (ap == 0 && w == 0) {
            char buff[MAX_LEN];
            sprintf(buff, "\033[1;34m%s\033[1;0m\n", files_list[idx]);
            bprintf(global_buffer, buff);
        } else {
            char buff[MAX_LEN];
            sprintf(buff, "%s\n", files_list[idx]);
            bprintf(global_buffer, buff);
        }
    } else if (file_stat.st_mode & S_IXUSR) { // checking if a file is executable
        // color green
        if (ap == 0 && w == 0) {
            char buff[MAX_LEN];
            sprintf(buff, "\033[1;32m%s\033[1;0m\n", files_list[idx]);
            bprintf(global_buffer, buff);
        } else {
            char buff[MAX_LEN];
            sprintf(buff, "%s\n", files_list[idx]);
            bprintf(global_buffer, buff);
        }
    } else { // else the file is regular
        // color white
        char buff[MAX_LEN];
        sprintf(buff, "%s\n", files_list[idx]);
        bprintf(global_buffer, buff);
    }
    return 1;
}

