#include "headers.h"

void proclore(char* pid) {
    char* path_stat = (char*) calloc(256, sizeof(char));
    sprintf(path_stat, "/proc/%d/stat", atoi(pid));
    // strcpy(path_stat, "/proc/");
    // strcat(path_stat, pid);
    // strcat(path_stat, "/status");

    char* path_maps = (char*) calloc(256, sizeof(char));
    sprintf(path_maps, "/proc/%d/maps", atoi(pid));
    // strcpy(path_maps, "/proc/");
    // strcat(path_maps, pid);
    // strcat(path_maps, "/maps");

    char* path_exe = (char*) calloc(256, sizeof(char));
    sprintf(path_exe, "/proc/%d/exe", atoi(pid));

    char* status;
    char* tty_nr;
    int bg_process;
    char* process_group;
    unsigned long virtual_address;
    char executable_path[MAX_LEN];

    char* data = (char*) calloc(100000, sizeof(char));
    char** data_array;
    char* buffer = (char*) calloc(50000, sizeof(char));

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
        free_tokens(data_array);
    }

    FILE* fptr2 = fopen(path_maps, "r");
    if (fptr2 != NULL) {
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

    free(buffer);
    free(data);
    free(path_stat);
    free(path_maps);
    free(path_exe);
}