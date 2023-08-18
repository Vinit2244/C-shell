#include "headers.h"

int main()
{
    char home_directory[MAX_LEN + 1]; // stores the absolute path to the home directory
    char cwd[MAX_LEN + 1];            // stores the absolute path to the current working directory
    char prev_dir[MAX_LEN + 1];       // stores the absolute path of the latest previous directory (used in warp -)

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
        
        int idx = 0;
        char* curr_command = list_of_commands[idx];
        while (curr_command[0] != '\0') {

            // ================= Printing the commands =================
            // printf("Command %d: %s\n", idx + 1, curr_command);

            // Different commands
// ===================================================================================
            // warp
            char warp_str[5] = "warp";
            int warp_flag = 1;

            for (int i = 0; i < 4; i++) {
                if (warp_str[i] == curr_command[i]) continue;
                else {
                    warp_flag = 0;
                    break;
                }
            }

            // checking if warp command is present
            if (warp_flag) {
                char** path_tokens = generate_tokens(curr_command, ' ');
                int no_of_arguments = 0;
                while(path_tokens[no_of_arguments] != NULL) {
                    no_of_arguments++;
                }
                no_of_arguments--;
                if (no_of_arguments == 0) {
                    char* c = "~";
                    warp(cwd, c, prev_dir, home_directory);
                } else {
                    for (int i = 1; i <= no_of_arguments; i++) {
                        warp(cwd, path_tokens[i], prev_dir, home_directory);
                    }
                }
                free_tokens(path_tokens);
            }

// ===================================================================================
            // peek
            char peek_str[5] = "peek";
            int peek_flag = 1;

            for (int i = 0; i < 4; i++) {
                if (peek_str[i] == curr_command[i]) continue;
                else {
                    peek_flag = 0;
                    break;
                }
            }

            // checking if peek command is present
            if (peek_flag) {
                char** argument_tokens = generate_tokens(curr_command, ' ');
                int no_of_arguments = 0;
                while(argument_tokens[no_of_arguments] != NULL) {
                    no_of_arguments++;
                }
                no_of_arguments--;

                char path[MAX_LEN];

                if (no_of_arguments == 0) {
                    for (int i = 0; i < strlen(cwd); i++) {
                        path[i] = cwd[i];
                    }
                    path[strlen(cwd)] = '\0';
                    peek(path, 0, 0, cwd, home_directory, prev_dir);
                } else {
                    char minus_a[3] = "-a";
                    char minus_l[3] = "-l";
                    char minus_al[4] = "-al";
                    char minus_la[4] = "-la";

                    int a = 0;
                    int l = 0;

                    int path_flag = 0;
                    for (int i = 1; i <= no_of_arguments; i++) {
                        if (strcmp(argument_tokens[i], minus_a) == 0) {
                            a = 1;
                        } else if (strcmp(argument_tokens[i], minus_l) == 0) {
                            l = 1;
                        } else if (strcmp(argument_tokens[i], minus_la) == 0 || strcmp(argument_tokens[i], minus_al) == 0) {
                            a = 1;
                            l = 1;
                        } else {
                            path_flag = 1;
                            for (int j = 0; j < strlen(argument_tokens[i]); j++) {
                                path[j] = argument_tokens[i][j];
                            }
                            path[strlen(argument_tokens[i])] = '\0';
                        }
                    }

                    peek(path, a, l, cwd, home_directory, prev_dir);
                }
                // if (no_of_arguments == 0) {
                //     char* c = "~";
                //     warp(cwd, c, prev_dir, home_directory);
                // } else {
                //     for (int i = 1; i <= no_of_arguments; i++) {
                //         warp(cwd, path_tokens[i], prev_dir, home_directory);
                //     }
                // }
                free_tokens(argument_tokens);
            }

// ===================================================================================
            idx++;
            curr_command = list_of_commands[idx];
        }
        
        free(input);
        free_commands_list(list_of_commands);
    }
    return 0;
}
