#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <pwd.h>
#include <unistd.h>
#include <stdlib.h>

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
        tokens_array[i] = (char*) calloc(1000000, sizeof(char));
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

int main(int argc, char* argv[]) {
    printf("Enter the process id: ");
    char* pid = (char*) calloc(100, sizeof(char));
    scanf("%s", pid);

    char* path_stat = (char*) calloc(256, sizeof(char));
    sprintf(path_stat, "/proc/%d/stat", atoi(pid));

    char* path_maps = (char*) calloc(256, sizeof(char));
    sprintf(path_maps, "/proc/%d/maps", atoi(pid));

    char* path_exe = (char*) calloc(256, sizeof(char));
    sprintf(path_exe, "/proc/%d/exe", atoi(pid));

    char* status;
    char* tty_nr;
    int bg_process;
    char* process_group;
    unsigned long virtual_address;
    char* executable_path[1000000];

    char* data = (char*) calloc(100000, sizeof(char));
    char** data_array;
    char* buffer;

    FILE *fptr = fopen(path_stat, "r");
    if (fptr == NULL) {
        printf("No such process with process id %s running\n", pid);
    } else {
        fscanf(fptr, " %[^\n]", data);
        data_array = generate_tokens(data, ' ');
        status = data_array[2];
        tty_nr = data_array[6];
        bg_process = 0;
        if (strcmp(tty_nr, "0") == 0) {
            bg_process = 1;
        }
        process_group = data_array[4];
        fclose(fptr);
    }

    FILE* fptr2 = fopen(path_maps, "r");
    if (fptr2 != NULL) {
        buffer = (char*) calloc(50000, sizeof(char));
        unsigned long start;
        unsigned long end;
        fgets(buffer, 50000, fptr2);
        sscanf(buffer, "%lx-%lx", &start, &end);
        virtual_address = start;
    }

    printf("pid : %s\n", pid);
    printf("process status : %s", status);
    if (bg_process == 1) {
        printf("\n");
    } else {
        printf("+\n");
    }
    printf("Process group : %s\n", process_group);
    printf("Virtual memory : %lu\n", virtual_address);


    ssize_t length = readlink(path_exe, executable_path, sizeof(executable_path) - 1);
    executable_path[length] = '\0';
    printf("executable path : %s\n", executable_path);

    // free(buffer);
    // free_tokens(args);
    // free_tokens(data_array);
    // free(data);
    // free(path_stat);
    // free(path_maps);
    return 0;
}