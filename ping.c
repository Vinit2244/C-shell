#include "headers.h"

int ping_sig(int pid, int sig, int ap, int w) {
    // checking if the process exists
    int result = kill(pid, 0);

    if (result == 0) {
        // process exists
        int response = kill(pid, sig);
        if (response == 0) {
            char buff[MAX_LEN] = {0};
            sprintf(buff, "Sent signal %d to process with pid %d\n", sig, pid);
            bprintf(global_buffer, buff);

            // if (kill(pid, 0) != 0) {
            //     printf("hi\n");
            //     LL_Node trav = LL->first;
            //     while (trav != NULL) {
            //         if (trav->pid == pid) {
            //             free_node(trav);
            //             break;
            //         }
            //         trav = trav->next;
            //     }
            // }

            LL_Node trav = LL->first;
            while (trav != NULL) {
                if (trav->pid == pid) {
                    int cstatus;
                    int res = waitpid(pid, &cstatus, WNOHANG);

                    if (res == pid) {
                        free_node(trav);
                    }


                    // char path_stat[256] = {0};
                    // sprintf(path_stat, "/proc/%d/stat", pid);
                    // char* status;
                    // int f = 0;

                    // FILE *fptr = fopen(path_stat, "r");
                    // if (fptr == NULL) {
                    //     if (ap == 0 && w == 0) {
                    //         char buff[MAX_LEN] = {0};
                    //         sprintf(buff, "\033[1;31mNo such process with process id %d running\033[1;0m\n", pid);
                    //         bprintf(global_buffer, buff);
                    //     } else {
                    //         char buff[MAX_LEN] = {0};
                    //         sprintf(buff, "No such process with process id %d running\n", pid);
                    //         bprintf(global_buffer, buff);
                    //     }
                    // } else {
                    //     char data[100000];
                    //     fscanf(fptr, " %[^\n]", data);
                    //     char** data_array = generate_tokens(data, ' ');
                    //     status = data_array[2];

                    //     printf("hi\n");

                    //     if (strcmp(status, "Z") == 0) {
                    //         printf("hi1\n");
                    //         free_node(trav);
                    //         break;
                    //     }

                    //     // if (strcmp(status, "R") == 0) {
                    //     //     f = -1;
                    //     // } else if (strcmp(status, "S") == 0 || strcmp(status, "D") == 0 || strcmp(status, "T") == 0) {
                    //     //     f = -2;
                    //     // }

                    //     fclose(fptr);
                    //     free_tokens(data_array);
                    // }
                    // // if (f != 0) {
                    // //     trav->flag = f;
                    // // } else if (f == 0) {
                    // //     free_node(trav);
                    // // }
                    // // break;
                    break;
                }
                trav = trav->next;
            }
        } else {
            if (ap == 1 || w == 1) {
                bprintf(global_buffer, "kill: could not send signal\n");
            } else {
                bprintf(global_buffer, "\033[1;31mkill: could not send signal\033[1;0m\n");
            }
            return 0;
        }
    } else {
        // process does not exist
        if (ap == 1 || w == 1) {
            bprintf(global_buffer, "No such process exists\n");
        } else {
            bprintf(global_buffer, "\033[1;31mNo such process exists\033[1;0m\n");
        }
        return 0;
    }
    return 1;
}

