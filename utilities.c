#include "headers.h"

void free_tokens(char** tokens) {
    int idx = 0;
    while (tokens[idx] != NULL) {
        free(tokens[idx]);
        idx++;
    }
    free(tokens);
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

// Removes the leading and trailing spaces/tabs from the input string and outputs the new modified string
void remove_leading_and_trailing_spaces(char* str) {
    // removing leading spaces 
    int front_index = 0;
    while (str[front_index] == ' ' || str[front_index] == '\t') {
        front_index++;
    }
    int rear_index = strlen(str) - 1;
    while (str[rear_index] == ' ' || str[rear_index] == '\t') {
        rear_index--;
    }
    if (rear_index < front_index) {
        str[0] = '\0';
        return;
    }
    int len = rear_index - front_index + 1;
    char* final = (char*) malloc(sizeof(char) * (len + 1));
    for (int i = 0; i < len; i++) {
        final[i] = str[front_index++];
    }
    final[len] = '\0';
    for (int i = 0; i < len + 1; i++) {
        str[i] = final[i];
    }
    free(final);
}

char* generate_new_path(char* cwd, char* path, char* prev_dir, char* home_dir) {
    int success = 1;

    char tilde[2]       = "~";
    char dash[2]        = "-";
    char double_dots[3] = "..";
    char dot[2]         = ".";
    char base_folder[2] = "/";

    char* new_path = (char*) calloc(MAX_LEN, sizeof(char));
    strcpy(new_path, cwd);

    char** dir_tokens = generate_tokens(path, '/');
    int dir_tokens_idx = 0;

    while (dir_tokens[dir_tokens_idx] != NULL) {
        char* curr_token = dir_tokens[dir_tokens_idx];
        if (strcmp(curr_token, tilde) == 0) {
            // change cwd to home
            strcpy(new_path, home_dir);
        } else if (strcmp(curr_token, dash) == 0) {
            // change cwd to prev
            // checking if the warp - command is called just after the first initiation
            if (prev_dir[0] == '\0') {
                success = 2;
                printf("OLDPWD not set\n");
                // don't change the cwd
            } else {
                strcpy(new_path, prev_dir);
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

int str_to_int(char* str) {
    int ans = 0;
    int idx = 0;
    while (str[idx] != '\0') {
        int digit;
        switch(str[idx]) {
            case '1':
                digit = 1;
                break;
            case '2':
                digit = 2;
                break;
            case '3':
                digit = 3;
                break;
            case '4':
                digit = 4;
                break;
            case '5':
                digit = 5;
                break;
            case '6':
                digit = 6;
                break;
            case '7':
                digit = 7;
                break;
            case '8':
                digit = 8;
                break;
            case '9':
                digit = 9;
                break;
            case '0':
                digit = 0;
                break;
            default:
                return -1;
        }
        ans *= 10;
        ans += digit;
    }
    return ans;
}

char* remove_extension(char* file_name) {
    char* final = (char*) calloc(MAX_LEN, sizeof(char));
    int idx = 0;
    while (file_name[idx] != '\0' && file_name[idx] != '.') {
        final[idx] = file_name[idx];
        idx++;
    }
    final[idx] = '\0';
    return final;
}

char* relative_path(char* complete_path, char* base_dir_path) {
    char* rel_path = (char*) calloc(MAX_LEN, sizeof(char));
    int idx = 0;
    for (int i = 0; i < strlen(complete_path); i++) {
        if (complete_path[i] == base_dir_path[i]) {
            continue;
        }
        idx = i;
        break;
    }
    rel_path[0] = '.';
    rel_path[1] = '/';
    for (int i = idx + 1; i < strlen(complete_path); i++) {
        rel_path[i - idx + 1] = complete_path[i];
    }
    return rel_path;
}

