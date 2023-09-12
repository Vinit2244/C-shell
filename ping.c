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

            LL_Node trav = LL->first;
            while (trav != NULL) {
                int cstatus;
                if (waitpid(trav->pid, &cstatus, WNOHANG) == trav->pid) {
                    free_node(trav);
                    break;
                }
                trav = trav->next;
            }
        } else {
            fprintf(stderr, "\033[1;31mkill: could not send signal\033[1;0m\n");
            return 0;
        }
    } else {
        // process does not exist
        fprintf(stderr, "\033[1;31mNo such process exists\033[1;0m\n");
        return 0;
    }
    return 1;
}

