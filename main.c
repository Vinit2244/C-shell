#include "headers.h"

int main()
{
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
    
    // Keep accepting commands
    while (1)
    {
        input(NULL, home_directory, cwd, prev_dir, 1);
    }
    return 0;
}
