#include "headers.h"

int main()
{
    char home_directory[10000];
    getcwd(home_directory, 9999);
    // Keep accepting commands
    while (1)
    {
        // Print appropriate prompt with username, systemname and directory before accepting input
        prompt(home_directory);
        char* input = (char*) calloc(5000, sizeof(char));
        fgets(input, 4096, stdin);

        char** list_of_commands = get_list_of_commands(input); // contains the list of all commands as separate strings in the form of a 2D array
        /*
        ================= Printing the commands =================
        int idx = 0;
        char* curr_command = list_of_commands[idx];
        while (curr_command != NULL) {
            printf("Command %d: %s\n", idx + 1, curr_command);
            idx++;
            curr_command = list_of_commands[idx];
        }
        */
        
        free(input);
        free_commands_list(list_of_commands);
    }
    return 0;
}
