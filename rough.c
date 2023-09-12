#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

void enableRawMode() {
    struct termios raw;
    if (tcgetattr(STDIN_FILENO, &raw) == -1) {
        perror("tcgetattr");
        exit(1);
    }
    raw.c_lflag &= ~(ECHO | ICANON);
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
        perror("tcsetattr");
        exit(1);
    }
}

void disableRawMode() {
    struct termios original;
    if (tcgetattr(STDIN_FILENO, &original) == -1) {
        perror("tcgetattr");
        exit(1);
    }
    original.c_lflag |= (ECHO | ICANON);
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &original) == -1) {
        perror("tcsetattr");
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3 || argv[1][0] != '-' || argv[1][1] != 'n') {
        fprintf(stderr, "Usage: %s -n [time_arg]\n", argv[0]);
        return 1;
    }

    int time_arg = atoi(argv[2]);

    if (time_arg <= 0) {
        fprintf(stderr, "Invalid time_arg\n");
        return 1;
    }

    enableRawMode();
    pid_t pid;

    while (1) {
        if ((pid = fork()) == -1) {
            perror("fork");
            exit(1);
        } else if (pid == 0) {
            // Child process: Print the process ID and exit
            printf("%d\n", getpid());
            exit(0);
        } else {
            // Parent process: Wait for [time_arg] seconds
            sleep(time_arg);
        }
    }

    // Disable raw mode before exiting
    disableRawMode();

    return 0;
}
