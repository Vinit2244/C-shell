#include "headers.h"

int print_info_pid(char** argument_tokens, int no_of_arguments, int w, int ap, int ip, char* input_file_name_redirection) {
    char* pid = (char*) calloc(50, sizeof(char));
    if (no_of_arguments == 0 && ip == 0) {         // if no arguments are passed than print the details of the termianl process
        int curr_pid = getpid();        // getting the pid of the current running process
        sprintf(pid, "%d", curr_pid);   // writing the pid in the form of a string to pass
    } else if (no_of_arguments == 0 && ip == 1) {
        char inp_buff[999999] = {0};

        char file_path[MAX_LEN];
        if (input_file_name_redirection[0] != '/') {
            strcpy(file_path, cwd);
            strcat(file_path, "/");
            strcat(file_path, input_file_name_redirection);
        } else if (input_file_name_redirection[0] == '/') {
            strcpy(file_path, input_file_name_redirection);
        }

        int fd = open(file_path, O_RDONLY);
        if (fd < 0) {
            fprintf(stderr, "\033[1;31mError in opening input file\033[1;0m\n");
            return 0;
        } else {
            int bytes_read = read(fd, inp_buff, 999998);
            if (bytes_read < 0) {
                fprintf(stderr, "\033[1;31mError in reading\033[1;0m\n");
                close(fd);
                return 0;
            } else {
                close(fd);
                if (inp_buff[strlen(inp_buff) - 1] == '\n') {
                    inp_buff[strlen(inp_buff) - 1] = '\0';
                }
                strcpy(pid, inp_buff);
            }
        }
    } else { // if pid of the process is provided
        strcpy(pid, argument_tokens[1]);
    }
    proclore(pid, ap, w);
    free(pid);
}

void proclore(char* pid, int ap, int w) {
    char path_stat[256] = {0};
    sprintf(path_stat, "/proc/%d/stat", atoi(pid));

    char path_maps[256] = {0};
    sprintf(path_maps, "/proc/%d/maps", atoi(pid));

    char path_exe[256] = {0};
    sprintf(path_exe, "/proc/%d/exe", atoi(pid));

    char* status;
    char* tty_nr;
    char* process_group;
    unsigned long virtual_address;
    char executable_path[MAX_LEN];

    FILE *fptr = fopen(path_stat, "r");
    if (fptr == NULL) {
        fprintf(stderr, "\033[1;31mNo such process with process id %s running\033[1;0m\n", pid);
        return;
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
        char buffer[50000];
        unsigned long start;
        unsigned long end;
        fgets(buffer, 50000, fptr2);
        sscanf(buffer, "%lx-%lx", &start, &end);
        virtual_address = start;
    } else {
        fprintf(stderr, "\033[1;31mproclore: cannot open /proc/pid/maps\033[1;0m\n");
        return;
    }

    char buff[MAX_LEN] = {0};
    sprintf(buff, "pid : %s\n", pid);
    bprintf(global_buffer, buff);

    char buff2[MAX_LEN] = {0};
    sprintf(buff2, "Process status : %s", status);
    bprintf(global_buffer, buff2);

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
        bprintf(global_buffer, "\n");
    } else {
        bprintf(global_buffer, "+\n");
    }
    char buff3[MAX_LEN] = {0};
    sprintf(buff3, "Process group : %s\n", process_group);
    bprintf(global_buffer, buff3);

    char buff4[MAX_LEN] = {0};
    sprintf(buff4, "Virtual memory : %lu\n", virtual_address);
    bprintf(global_buffer, buff4);
    
    ssize_t length = readlink(path_exe, executable_path, sizeof(executable_path) - 1);
    executable_path[length] = '\0';

    char buff5[MAX_LEN + 100] = {0};
    sprintf(buff5, "Executable path : %s\n", executable_path);
    bprintf(global_buffer, buff5);
}

