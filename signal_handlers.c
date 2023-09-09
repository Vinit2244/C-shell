#include "headers.h"

// ctrl + z
void handle_sigtstp(int sig) {
    printf("\nctrl z\n");
    // prompt(home_directory, cwd, &t, last_command);
    // while (1)
    // {
    //     input(NULL, home_directory, cwd, prev_dir, 1, last_command, &t, 0, 0, 0, NULL);
    // }
}

// ctrl + c
void handle_sigint(int sig) {
    if (global_fg_pid == -1) {
        // do nothing
    } else {
        kill(global_fg_pid, 2);
    }
}

// ctrl + d
void handle_ctrl_d() {

}

