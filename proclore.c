#include "headers.h"

void proclore(char* pid) {
    char* path_stat = (char*) calloc(256, sizeof(char));
    sprintf(path_stat, "/proc/%d/stat", atoi(pid));

    char* path_maps = (char*) calloc(256, sizeof(char));
    sprintf(path_maps, "/proc/%d/maps", atoi(pid));

    char* path_exe = (char*) calloc(256, sizeof(char));
    sprintf(path_exe, "/proc/%d/exe", atoi(pid));

    char* status;
    char* tty_nr;
    char* process_group;
    unsigned long virtual_address;
    char executable_path[MAX_LEN];

    FILE *fptr = fopen(path_stat, "r");
    if (fptr == NULL) {
        printf("\033[1;31mNo such process with process id %s running\033[1;0m\n", pid);
    } else {
        char data[100000];
        fscanf(fptr, " %[^\n]", data);
        char** data_array = generate_tokens(data, ' ');
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
        char buffer[5000];
        unsigned long start;
        unsigned long end;
        fgets(buffer, 50000, fptr2);
        sscanf(buffer, "%lx-%lx", &start, &end);
        virtual_address = start;
    } else {
        printf("\033[1;31mproclore: cannot open /proc/pid/maps\033[1;0m\n");
    }

    printf("pid : %s\n", pid);
    printf("process status : %s", status);

    int background_process = 0;
    LL_Node trav = LL->first;
    while (trav != NULL) {
        if (trav->pid == atoi(pid)) {
            background_process = 1;
            break;
        }
        trav = trav->next;
    }
    if (background_process == 1) {
        printf("\n");
    } else {
        printf("+\n");
    }
    printf("Process group : %s\n", process_group);
    printf("Virtual memory : %lu\n", virtual_address);
    
    ssize_t length = readlink(path_exe, executable_path, sizeof(executable_path) - 1);
    executable_path[length] = '\0';
    printf("executable path : %s\n", executable_path);

    free(path_stat);
    free(path_maps);
    free(path_exe);
}