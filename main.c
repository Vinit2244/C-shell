#include "headers.h"

LL_Head LL;
char* bg_process_buffer;

char* global_buffer;
int global_buffer_empty;

// char* home_directory;
// char* cwd;
// char* prev_dir;
// char* last_command;
// int t;

int main()
{
    struct sigaction sa;
    sa.sa_handler = &handle_sigtstp; // Ctrl + Z or Cmd + Z
    sa.sa_flags = SA_RESTART;
    sigaction(SIGTSTP, &sa, NULL);

    struct sigaction sa2;
    sa2.sa_handler = &handle_sigint; // Ctrl + C or Cmd + C
    sa2.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa2, NULL);


    global_buffer_empty = 1;
    global_buffer = (char*) calloc(9999999, sizeof(char));
    bg_process_buffer = calloc(9999999, sizeof(char));

    // char* home_directory = (char*) calloc(MAX_LEN + 1, sizeof(char));
    // char* cwd = (char*) calloc(MAX_LEN + 1, sizeof(char));
    // char* prev_dir = (char*) calloc(MAX_LEN + 1, sizeof(char));

    char home_directory[MAX_LEN + 1]; // stores the absolute path to the home directory
    char cwd[MAX_LEN + 1];            // stores the absolute path to the current working directory
    char prev_dir[MAX_LEN + 1];       // stores the absolute path of the latest previous directory (used in warp -)

    char* buff1 = home_directory;
    char* buff2 = cwd;
    do {
        char* buff1 = getcwd(home_directory, MAX_LEN);
        char* buff2 = getcwd(cwd, MAX_LEN);          // initially current working directory will be same as the home_directory
    } while (buff1 == NULL || buff2 == NULL);

    prev_dir[0] = '\0';

    LL = create_LL();

    // last_command = (char*) calloc(MAX_LEN, sizeof(char));
    // t = 0;

    char* last_command = (char*) calloc(MAX_LEN, sizeof(char));
    int t = 0;
    // Keep accepting commands
    while (1)
    {
        input(NULL, home_directory, cwd, prev_dir, 1, last_command, &t, 0, 0, 0, NULL);
    }

    // free(home_directory);
    // free(cwd);
    // free(prev_dir);

    free(last_command);
    return 0;
}
