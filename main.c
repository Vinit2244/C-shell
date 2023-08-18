#include "headers.h"

int main()
{
    char home_directory[MAX_LEN + 1]; // stores the absolute path to the home directory
    char cwd[MAX_LEN + 1];            // stores the absolute path to the current working directory
    char prev_dir[MAX_LEN + 1];       // stores the absolute path of the latest previous directory (used in wrap -)

    char* buff1 = home_directory;
    char* buff2 = cwd;
    char* buff3 = prev_dir;
    do {
        char* buff1 = getcwd(home_directory, MAX_LEN);
        char* buff2 = getcwd(cwd, MAX_LEN);          // initially current working directory will be same as the home_directory
        char* buff3 = getcwd(prev_dir, MAX_LEN);
    } while (buff1 == NULL || buff2 == NULL);
    
    
    // Keep accepting commands
    while (1)
    {
        // Print appropriate prompt with username, systemname and directory before accepting input
        prompt(home_directory, cwd);
        char* input = (char*) calloc(5000, sizeof(char));
        fgets(input, 4096, stdin);
        input[strlen(input)] = '\0';

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
            // printf("Command %d: %s\n", idx + 1, curr_command);
            char* wrap_str = "wrap ";
            int wrap_flag = 1;
            for (int i = 0; i < 5; i++) {
                if (wrap_str[i] == curr_command[i]) continue;
                else {
                    wrap_flag = 0;
                    break;
                }
            }
            if (wrap_flag) {
                // printf("%s\n", &curr_command[5]);
                wrap(&curr_command[5], cwd, prev_dir, home_directory);
            }
            idx++;
            curr_command = list_of_commands[idx];
        }
        
        free(input);
        free_commands_list(list_of_commands);
    }
    return 0;
}
