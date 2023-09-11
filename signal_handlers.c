#include "headers.h"

// ctrl + z
void handle_sigtstp(int sig) {
    if (global_fg_pid == -1) {
        // do nothing
    } else {
        kill(global_fg_pid, SIGTSTP);
        insert_node_in_LL(global_fg_pid, -2, fg_command_name);
    }
}

// ctrl + c
void handle_sigint(int sig) {
    if (global_fg_pid == -1) {
        // do nothing
    } else {
        kill(global_fg_pid, SIGKILL);
    }
}

// ctrl + d
void handle_ctrl_d() {
    printf("received ctrl d\n");
}

