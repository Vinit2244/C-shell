#include "headers.h"

// ctrl + z
void handle_sigtstp(int sig) {
    if (global_fg_pid == -1) {
        // do nothing
    } else {
        kill(global_fg_pid, SIGTSTP);
        insert_node_in_LL(global_fg_pid, -2, fg_command_name);
        printf("\n");
    }
}

// ctrl + c
void handle_sigint(int sig) {
    if (global_fg_pid == -1) {
        // do nothing
    } else {
        kill(global_fg_pid, SIGKILL);
        printf("\n");
    }
}

// ctrl + d
void handle_ctrl_d() {
    LL_Node trav = LL->first;
    while (trav != NULL) {
        kill(trav->pid, SIGKILL);
        trav = trav->next;
    }
    exit(0);
}
