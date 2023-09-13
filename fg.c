#include "headers.h"

int bring_process_to_foreground(char** argument_tokens) {
    int pid = atoi(argument_tokens[1]);

    int result = kill(pid, 0);

    if (result == 0) {
        int cstatus;
        tcsetpgrp(STDIN_FILENO, pid);
        kill(pid, SIGCONT);
        waitpid(pid, &cstatus, WUNTRACED);
    } else {
        fprintf(stderr, "\033[1;31mNo such process exists\033[1;0m\n");
        return 0;
    }
}