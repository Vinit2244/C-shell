#include "headers.h"

void handle_sigtstp(int sig) {
    printf("\nctrl z\n");
    // prompt(home_directory, cwd, &t, last_command);
    // while (1)
    // {
    //     input(NULL, home_directory, cwd, prev_dir, 1, last_command, &t, 0, 0, 0, NULL);
    // }
}

void handle_sigint(int sig) {
    printf("\nctrl c\n");
    // prompt(home_directory, cwd, &t, last_command);
    // while (1)
    // {
    //     input(NULL, home_directory, cwd, prev_dir, 1, last_command, &t, 0, 0, 0, NULL);
    // }
}

