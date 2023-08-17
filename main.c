#include "headers.h"

int main()
{
    char home_directory[10000]; // stores the absolute path to the home directory
    char cwd[10000];            // stores the absolute path to the current working directory
    
    char* buff1 = home_directory;
    char* buff2 = cwd;
    do {
        char* buff1 = getcwd(home_directory, 9999);
        char* buff2 = getcwd(cwd, 9999);          // initially current working directory will be same as the home_directory
    } while (buff1 == NULL || buff2 == NULL);
    
    
    // Keep accepting commands
    while (1)
    {
        // Print appropriate prompt with username, systemname and directory before accepting input
        prompt(home_directory, cwd);
        char* input = (char*) calloc(5000, sizeof(char));
        fgets(input, 4096, stdin);

        for (int i = 0; i < strlen(input); i++) {
            if (input[i] != '\n') continue;
            else {
                input[i] = '\0';
            }
        }

        char** list_of_commands = get_list_of_commands(input); // contains the list of all commands as separate strings in the form of a 2D array
        
        // ================= Printing the commands =================
        int idx = 0;
        char* curr_command = list_of_commands[idx];
        while (curr_command[0] != '\0') {
            printf("Command %d: %s\n", idx + 1, curr_command);
            idx++;
            curr_command = list_of_commands[idx];
        }
        
        free(input);
        free_commands_list(list_of_commands);
    }
    return 0;
}
