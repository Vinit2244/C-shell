#include "headers.h"

LL_Head LL;                 // Linked List to store the pids and names of background processes spawned by my shell
char* bg_process_buffer;    

char* global_buffer;        // Temporarily stores output for redirection
int global_buffer_empty;    // 1 if globa_buffer is empty otherwise 0
char* fg_command_name;      // Name of the command currently running in foreground

char* home_directory;       // Directory where the shell is first spawned
char* cwd;                  // Current working directory
char* prev_dir;             // Directory we were previously on
char* last_command;         // Last executed command
int t;                      // Time taken for executing a command

int shell_pid;
int bg_gpid;

int main()
{
    shell_pid = getpid();
    bg_gpid = -1;
    fg_command_name = NULL; // Initially no foreground process is running by my shell

    struct sigaction sa;
    sa.sa_handler = &handle_sigtstp; // Ctrl + Z (Windows/Linux) or Cmd + Z (Mac)
    sa.sa_flags = SA_RESTART;
    sigaction(SIGTSTP, &sa, NULL);   // Ctrl + Z sends SIGTSTP signal (Signal Stop) - sends foreground process to background

    struct sigaction sa2;
    sa2.sa_handler = &handle_sigint; // Ctrl + C (Windows/Linux) or Cmd + C (Mac)
    sa2.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa2, NULL);   // Ctrl + C sends SIGINT signal (Signal Interrupt) - terminates the currently running foreground process


    global_buffer_empty = 1;
    global_buffer = (char*) calloc(9999999, sizeof(char));
    bg_process_buffer = calloc(9999999, sizeof(char));

    home_directory = (char*) calloc(MAX_LEN + 1, sizeof(char)); // stores the absolute path to the home directory
    cwd = (char*) calloc(MAX_LEN + 1, sizeof(char));            // stores the absolute path to the current working directory
    prev_dir = (char*) calloc(MAX_LEN + 1, sizeof(char));       // stores the absolute path of the latest previous directory (used in warp -)

    char* buff1 = home_directory;
    char* buff2 = cwd;
    do {
        char* buff1 = getcwd(home_directory, MAX_LEN);
        char* buff2 = getcwd(cwd, MAX_LEN);          // initially current working directory will be same as the home_directory
    } while (buff1 == NULL || buff2 == NULL);

    prev_dir[0] = '\0';

    LL = create_LL();

    last_command = (char*) calloc(MAX_LEN, sizeof(char));
    t = 0;

    // Keep accepting commands
    while (1)
    {
        int fd_in = dup(STDIN_FILENO);
        int fd_out = dup(STDOUT_FILENO);
        input(NULL, 1, 0);
        dup2(fd_in, STDIN_FILENO);
        dup2(fd_out, STDOUT_FILENO);
        close(fd_in);
        close(fd_out);
    }

    free(home_directory);
    free(cwd);
    free(prev_dir);

    free(last_command);
    return 0;
}
