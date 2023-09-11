#include "headers.h"

int bg_process = 0; // Flag to mark if a process is background process or not
int start = 0;      // Variable to hold the start time of command execution
time_t tyme = 0;    // Variable to hold the time of execution

int global_fg_pid;

void input(char* command, char* home_directory, char* cwd, char* prev_dir, int store, char* last_command, int* t, int w, int ap, int ip, char* output_file_name_redirection, char* input_file_name_redirection) {

    global_fg_pid = -1; // -1 represents no foreground process is initiated by my terminal

    tyme = time(NULL) - start;  // current time - start time
    *t = tyme;                  // global variable to hold the time of last executed command

    start = 0;                  // reset start time to zero
    tyme = 0;                   // reset execution time to zero

    int overall_success = 1;    // flag to mark if the complete command ran without failing
    int pastevents_present = 0; // flag to mark if pastevents command is present

    char* input_string = (char*) calloc(5000, sizeof(char));
    input_string[0] = '\0';
    
    if (command == NULL) {  // command is NULL if the input function is called from main and not from pastevents execute function
        store = 1;          // flag to reflect if the command inputted is to be stored or not

        // Print appropriate prompt with username, systemname and directory before accepting input
        prompt(home_directory, cwd, t, last_command);
        
        fgets(input_string, 4096, stdin);
        input_string[strlen(input_string)] = '\0';

        // removing endline character
        for (int i = 0; i < strlen(input_string); i++) {
            if (input_string[i] != '\n') continue;
            else {
                input_string[i] = '\0';
            }
        }
        // check through the linked list of all the background processes that were running and print the ones which are done
        check_and_print();
        
        if (bg_process_buffer[0] == '\0') {
            // don't print anything
        } else {
            printf("%s", bg_process_buffer);
            bg_process_buffer[0] = '\0';
        }

    } else {        // input is called by pastevents execute
        store = 0;  // don't store the command as it was called by pastevents execute
        for (int i = 0; i < strlen(command); i++) {
            input_string[i] = command[i];
        }
        input_string[strlen(command)] = '\0';
    }

    last_command[0] = '\0';

    char** list_of_commands = get_list_of_commands(input_string); // contains the list of all commands as separate strings in the form of a 2D array

    start = time(NULL); // record the time before we start executing the commands

    int idx = 0;        // index variable to hold the index of command to be executed
    char* curr_command = list_of_commands[idx];
    while (curr_command[0] != '\0') {
        // Checking if the command involves piping
        int pipe_flag = 0;
        for (int i = 0; i < strlen(curr_command); i++) {
            if (curr_command[i] == '|') {
                pipe_flag = 1;
                break;
            }
        }

        int append_flag = 0; // output redirection '>>'
        char* app_cmd = (char*) calloc(MAX_LEN, sizeof(char));
        char* to_file = (char*) calloc(MAX_LEN, sizeof(char));
        for (int i = 0; i < strlen(curr_command) - 1; i++) {
            if (curr_command[i] == '>' && curr_command[i + 1] == '>') {
                append_flag = 1;
                for (int j = 0; j < i; j++) {
                    app_cmd[j] = curr_command[j];
                }
                remove_leading_and_trailing_spaces(app_cmd);
                for (int j = i + 2; j < strlen(curr_command); j++) {
                    to_file[j - i - 2] = curr_command[j];
                }
                remove_leading_and_trailing_spaces(to_file);
                break;
            }
        }

        int write_flag = 0; // output redirection '>'
        for (int i = 0; i < strlen(curr_command); i++) {
            if (curr_command[i] == '>') {
                write_flag = 1;
                break;
            }
        }

        int inp_flag = 0; // input redirection '<'
        for (int i = 0; i < strlen(curr_command); i++) {
            if (curr_command[i] == '<') {
                inp_flag = 1;
                break;
            }
        }

        if (pipe_flag == 1) {
            char** sub_commands = generate_tokens(curr_command, '|');

            char temp_file_for_piping[MAX_LEN] = {0};
            strcpy(temp_file_for_piping, home_directory);
            strcat(temp_file_for_piping, "/a_a.txt");

            input(sub_commands[0], home_directory, cwd, prev_dir, store, last_command, t, 1, 0, 0, temp_file_for_piping, NULL);

            int k = 1;
            while (sub_commands[k] != NULL) {
                input(sub_commands[k], home_directory, cwd, prev_dir, store, last_command, t, 1, 0, 1, temp_file_for_piping, temp_file_for_piping);
                k++;
            }

            free_tokens(sub_commands);
            // return;
        } else if (inp_flag == 1) {
            if (write_flag == 0 && append_flag == 0) {
                char** files = generate_tokens(curr_command, '<');
                char* cmd = files[0];

                remove_leading_and_trailing_spaces(cmd);
                char** tokens = generate_tokens(cmd, ' ');
                int k = 0;
                while (tokens[k] != NULL) {
                    if (strcmp(tokens[k], "pastevents") == 0) {
                        pastevents_present = 1;
                        break;
                    }
                    k++;
                }
                free_tokens(tokens);

                char* inp_file = files[1];
                input(cmd, home_directory, cwd, prev_dir, store, last_command, t, 0, 0, 1, NULL, inp_file);

                free_tokens(files);
            } else {
                if (write_flag == 1) {
                    // assuming of the form cmd < input_file > output_file
                    char** tkns = generate_tokens(curr_command, '<');
                    char* cmd = tkns[0];

                    char** tkns2 = generate_tokens(tkns[1], '>');
                    char* input_file = tkns2[0];
                    char* output_file = tkns2[1];

                    input(cmd, home_directory, cwd, prev_dir, store, last_command, t, 1, 0, 1, output_file, input_file);

                    free_tokens(tkns2);
                    free_tokens(tkns);
                } else if (append_flag == 1) {

                }
            }
        } else if (write_flag == 1) {
            char** params = generate_tokens(curr_command, '>');
            char* cmd = params[0];

            // Checking for the presence of pastevents command
            remove_leading_and_trailing_spaces(cmd);
            char** tokens = generate_tokens(cmd, ' ');
            int k = 0;
            while (tokens[k] != NULL) {
                if (strcmp(tokens[k], "pastevents") == 0) {
                    pastevents_present = 1;
                    break;
                }
                k++;
            }
            free_tokens(tokens);

            char* output_file = params[1];
            input(cmd, home_directory, cwd, prev_dir, store, last_command, t, 1, 0, 0, output_file, NULL);
            free_tokens(params);
        } else if (append_flag == 1) {
            // Checking for the presence of pastevents command
            remove_leading_and_trailing_spaces(app_cmd);
            char** tokens = generate_tokens(app_cmd, ' ');
            int k = 0;
            while (tokens[k] != NULL) {
                if (strcmp(tokens[k], "pastevents") == 0) {
                    pastevents_present = 1;
                    break;
                }
                k++;
            }
            free_tokens(tokens);

            input(app_cmd, home_directory, cwd, prev_dir, store, last_command, t, 0, 1, 0, to_file, NULL);
            free(app_cmd);
            free(to_file);
        } else {
            // ================= Printing the commands =================
            // printf("Command %d: %s\n", idx + 1, curr_command);

            // tokenize the command based on strings
            char** argument_tokens = generate_tokens(curr_command, ' ');

            // calculating the number of arguments passed to the command
            int no_of_arguments = 0;
            while(argument_tokens[no_of_arguments] != NULL) {
                no_of_arguments++;
            }
            no_of_arguments--;

            // storing the name of commands in the last command array
            if (strlen(last_command) == 0) {
                strcpy(last_command, argument_tokens[0]);
            } else {
                strcat(last_command, ", ");
                strcat(last_command, argument_tokens[0]);
            }

            // Different commands
    // ===================================================================================
            // iMan
            /*
                Reads the Man pages of the specified command from man.he.net website and prints it
            */

            // checking if iMan command is present
            if (strcmp("iMan", argument_tokens[0]) == 0) {
                int exit_status = 0;
                if (no_of_arguments == 0) {
                    // Can read the name of the command from input redirection
                    if (ip == 1) {
                        char input_file_path[MAX_LEN];

                        if (input_file_name_redirection[0] != '/') {
                            strcpy(input_file_path, cwd);
                            strcat(input_file_path, "/");
                            strcat(input_file_path, input_file_name_redirection);
                        } else if (output_file_name_redirection[0] == '/') {
                            strcpy(input_file_path, input_file_name_redirection);
                        }

                        char inp_buff[MAX_LEN] = {0};
                        int inp_fd = open(input_file_path, O_RDONLY);

                        if (inp_fd < 0) {
                            // open failed
                            printf("\033[1;31mopen : %s\033[1;0m\n", strerror(errno));
                            exit_status = 0;
                        } else {
                            int bytes_read = read(inp_fd, inp_buff, MAX_LEN - 1);
                            if (bytes_read < 0) {
                                // read fails
                                printf("\033[1;31mread : cannot read from the file\033[1;0m\n");
                                exit_status = 0;
                            } else {
                                for (int r = 0; r < strlen(inp_buff); r++) {
                                    if (inp_buff[r] == '\n') {
                                        inp_buff[r] = '\0';
                                    }
                                }
                                exit_status = get_webpage(inp_buff);
                            }
                        }
                    } else {
                        // invalid arguments command name has to be provided
                        printf("\033[1;31miMan: no argument provided (command name required)\033[1;0m\n");
                    }
                } else if (no_of_arguments == 1) {
                    exit_status = get_webpage(argument_tokens[1]);
                } else {
                    printf("\033[1;31miMan: excess number of arguments provided\033[1;0m\n");
                    exit_status = 0;
                }
                
                io_redirection(ap, w, cwd, output_file_name_redirection);
                if (exit_status == 0) overall_success = 0;
            }
    // ===================================================================================
            // warp
            /*
                Changes the current working directory to the relative/absolute path provided
            */

            // checking if warp command is present
            else if (strcmp("warp", argument_tokens[0]) == 0) {
                int success = 1;            // flag to keep track if warp exited successfully or not
                if (ip == 1) {
                    // if input redirection is provided to warp it just goes back to home directory
                    // warp does not accept input redirection
                    int exit_code = warp(cwd, "~", prev_dir, home_directory, ap, w);
                    if (exit_code == 0) success = 0;
                } else {
                    if (no_of_arguments == 0) { // if no argument is passed then warp to the home directory
                        char* c = "~";
                        // returns 1 if successfull and returns 0 if some error occured
                        int exit_code = warp(cwd, c, prev_dir, home_directory, ap, w);
                        if (exit_code == 0) success = 0;
                    } else {                    // if multiple arguments are passed then warp to those arguments one by one (each argument treated as a separate command)
                        for (int i = 1; i <= no_of_arguments; i++) {
                            int exit_code = warp(cwd, argument_tokens[i], prev_dir, home_directory, ap, w);
                            if (exit_code == 0) success = 0;
                        }
                    }
                }

                io_redirection(ap, w, cwd, output_file_name_redirection);

                if (success == 0) overall_success = 0;
            }
    // ===================================================================================
            // peek
            /*
                Prints the files/directories present in the path of file provided
            */

            // checking if peek command is present
            else if (strcmp("peek", argument_tokens[0]) == 0) {
                char path[MAX_LEN] = {0}; // stores path of directory to peek into
                
                // copying cwd into path
                // if no argument is provided or input redirection is provided then just print the contents of the cwd
                // peek does not accept input redirection
                strcpy(path, cwd);

                int a = 0;
                int l = 0;

                if (no_of_arguments != 0) {
                    // valid flags
                    char minus_a[3] = "-a";
                    char minus_l[3] = "-l";
                    char minus_al[4] = "-al";
                    char minus_la[4] = "-la";

                    int path_flag = 0; // path to some directory is present or not
                    for (int i = 1; i <= no_of_arguments; i++) {
                        // checking which all flags are present
                        if (strcmp(argument_tokens[i], minus_a) == 0) {
                            a = 1;
                        } else if (strcmp(argument_tokens[i], minus_l) == 0) {
                            l = 1;
                        } else if (strcmp(argument_tokens[i], minus_la) == 0 || strcmp(argument_tokens[i], minus_al) == 0) {
                            a = 1;
                            l = 1;
                        } else {
                            // any argument not starting with "-" is treated as some path given
                            path_flag = 1;

                            // copying the provided path into the path array replacing cwd
                            for (int j = 0; j < strlen(argument_tokens[i]); j++) {
                                path[j] = argument_tokens[i][j];
                            }
                            path[strlen(argument_tokens[i])] = '\0';
                        }
                    }
                }
                // returns 1 if peeked successfully else returns 0
                int exit_code = peek(path, a, l, cwd, home_directory, prev_dir, ap, w);
                if (exit_code == 0) overall_success = 0;

                io_redirection(ap, w, cwd, output_file_name_redirection);
            }
    // ===================================================================================
            // pastevents
            /*
                Prints the pastevents that has been executed or executes some pastevents
            */

            // checking if pastevents command is present
            else if (strcmp("pastevents", argument_tokens[0]) == 0) {
                pastevents_present = 1;
                if (no_of_arguments == 0) { // no arguments are passed than just print the pastevents
                    pastevents(home_directory);
                } else {                    // if some argument is present
                    if (strcmp(argument_tokens[1], "purge") == 0) {
                        if (no_of_arguments > 1) {
                            overall_success = 0;
                            if (ap == 1 || w == 1) {
                                bprintf(global_buffer, "pastevents: ivalid arguments\n");
                            } else {
                                bprintf(global_buffer, "\033[1;31mpastevents: ivalid arguments\033[1;0m\n");
                            }
                        } else {
                            // clears the stored list
                            purge(ap, w, home_directory);
                        }
                    } else if (strcmp(argument_tokens[1], "execute") == 0) { // execute some pastevent whose event is given
                        if (no_of_arguments == 1 && ip == 0) {
                            // if no index is given which command to execute then show error
                            if (ap == 0 && w == 0) {
                                char buff[MAX_LEN] = {0};
                                sprintf(buff, "\033[1;31mpastevents: missing argument in \"%s\"\033[1;0m\n", curr_command);
                                bprintf(global_buffer, buff);
                            } else {
                                char buff[MAX_LEN] = {0};
                                sprintf(buff, "pastevents: missing argument in \"%s\"\n", curr_command);
                                bprintf(global_buffer, buff);
                            }
                        } else if (no_of_arguments == 1 && ip == 1) {
                            char number[MAX_LEN] = {0};
                            int num = 0;    // variable to store the index of command to execute
                            int flag = 1;   // valid index is passed between 1 and 15 (both inclusive)

                            char inp_buff[999999] = {0};

                            char file_path[MAX_LEN];
                            strcpy(file_path, cwd);
                            strcat(file_path, "/");
                            strcat(file_path, input_file_name_redirection);

                            int fd = open(file_path, O_RDONLY);
                            if (fd < 0) {
                                perror("Error in opeaning the input file");
                                overall_success = 0;
                            } else {
                                int bytes_read = read(fd, inp_buff, 999998);
                                if (bytes_read < 0) {
                                    perror("Error in reading");
                                    close(fd);
                                    overall_success = 0;
                                } else {
                                    close(fd);
                                    if (inp_buff[strlen(inp_buff) - 1] == '\n') {
                                        inp_buff[strlen(inp_buff) - 1] = '\0';
                                    }
                                    strcpy(number, inp_buff);
                                }
                            }
                            // converting char to int
                            // doing it the long way since some invalid string can also be given in argument so i had to handle it manually and not use atoi()
                            convert_to_int(number, &num, &flag, ap, w);
                            if (flag) {
                                // executing the command at the given index
                                int exit_code = execute(num, home_directory, cwd, prev_dir, store, last_command, t, ap, w);
                                if (exit_code == 0) overall_success = 0;
                            }
                        } else if (no_of_arguments == 2) {
                            char* number = argument_tokens[2];
                            int num = 0;    // variable to store the index of command to execute
                            int flag = 1;   // valid index is passed between 1 and 15 (both inclusive)
                            
                            // converting char to int
                            // doing it the long way since some invalid string can also be given in argument so i had to handle it manually and not use atoi()
                            convert_to_int(number, &num, &flag, ap, w);
                            if (flag) {
                                // executing the command at the given index
                                int exit_code = execute(num, home_directory, cwd, prev_dir, store, last_command, t, ap, w);
                                if (exit_code == 0) overall_success = 0;
                            }
                        } else {
                            // excess of arguments are passed
                            if (ap == 0 && w == 0) {
                                char buff[MAX_LEN] = {0};
                                sprintf(buff, "\033[1;31mpastevents: excess arguments in \"%s\"\033[1;0m\n", curr_command);
                                bprintf(global_buffer, buff);
                            } else {
                                char buff[MAX_LEN] = {0};
                                sprintf(buff, "pastevents: excess arguments in \"%s\"\n", curr_command);
                                bprintf(global_buffer, buff);
                            }
                        }
                    } else {
                        // invalid arguments are passed
                        if (ap == 0 && w == 0) {
                            char buff[MAX_LEN] = {0};
                            sprintf(buff, "\033[1;31mpastevents: invalid arguments in \"%s\"\033[1;0m\n", curr_command);
                            bprintf(global_buffer, buff);
                        } else {
                            char buff[MAX_LEN] = {0};
                            sprintf(buff, "pastevents: invalid arguments in \"%s\"\n", curr_command);
                            bprintf(global_buffer, buff);
                        }
                    }
                }
                io_redirection(ap, w, cwd, output_file_name_redirection);
            }
    // ===================================================================================
            // proclore
            /*
                Prints all the details about the process whose process id is passed as an argument
            */

            // checking if proclore command is present
            else if (strcmp("proclore", argument_tokens[0]) == 0) {
                char* pid = (char*) calloc(50, sizeof(char));
                if (no_of_arguments == 0 && ip == 0) {         // if no arguments are passed than print the details of the termianl process
                    int curr_pid = getpid();        // getting the pid of the current running process
                    sprintf(pid, "%d", curr_pid);   // writing the pid in the form of a string to pass
                } else if (no_of_arguments == 0 && ip == 1) {
                    char inp_buff[999999] = {0};

                    char file_path[MAX_LEN];
                    if (input_file_name_redirection[0] != '/') {
                        strcpy(file_path, cwd);
                        strcat(file_path, "/");
                        strcat(file_path, input_file_name_redirection);
                    } else if (input_file_name_redirection[0] == '/') {
                        strcpy(file_path, input_file_name_redirection);
                    }

                    int fd = open(file_path, O_RDONLY);
                    if (fd < 0) {
                        perror("Error in opeaning the input file");
                        overall_success = 0;
                    } else {
                        int bytes_read = read(fd, inp_buff, 999998);
                        if (bytes_read < 0) {
                            perror("Error in reading");
                            close(fd);
                            overall_success = 0;
                        } else {
                            close(fd);
                            if (inp_buff[strlen(inp_buff) - 1] == '\n') {
                                inp_buff[strlen(inp_buff) - 1] = '\0';
                            }
                            strcpy(pid, inp_buff);
                        }
                    }
                } else { // if pid of the process is provided
                    strcpy(pid, argument_tokens[1]);
                }
                proclore(pid, ap, w);

                io_redirection(ap, w, cwd, output_file_name_redirection);

                free(pid);
            }
    // ===================================================================================
            // seek
            /*
                Searches for a file/directory in the relative/absolute path provided
                Searches the entire depth of the tree starting from the provided path
                flags: -e: Checks if only one directory/file with the given name is found
                        (if file is found      : prints it's outpus)
                        (if directory is found : changes cwd to that directory)
                    -f: Checks only for files of specified name
                    -d: Checks only for directories of specified name
            */

            // checking if seek command is present
            else if (strcmp("seek", argument_tokens[0]) == 0) {
                char* base_dir = (char*) calloc(MAX_LEN, sizeof(char));
                char* file_name = (char*) calloc(MAX_LEN, sizeof(char));

                int d = 0;
                int f = 0;
                int e = 0;

                int base_dir_flag = 0;
                int valid_flags = 1;
                int file_name_flag = 0;

                int success = 1;

                if (no_of_arguments == 0) {
                    // file/directory name to be searched should be provided
                    if (ap == 1 || w == 1) {
                        bprintf(global_buffer, "seek: missing arguments\n");
                    }
                    else {
                        bprintf(global_buffer, "\033[1;31mseek: missing arguments\033[1;0m\n");
                    }
                    overall_success = 0;
                } else {
                    for (int i = 1; i <= no_of_arguments; i++) {
                        char* curr_argument = argument_tokens[i];

                        if (curr_argument[0] == '-') {
                            if (base_dir_flag == 1) {
                                // flags should be provided before providing the path to the base directory
                                if (ap == 1 || w == 1) {
                                    bprintf(global_buffer, "seek: Invalid Arguments\n");
                                }
                                else {
                                    bprintf(global_buffer, "\033[1;31mseek: Invalid Arguments\033[1;0m\n");
                                }
                                overall_success = 0;
                            } else {
                                // checking for flags present
                                if (strcmp(curr_argument, "-d") == 0) {
                                    d = 1;
                                } else if (strcmp(curr_argument, "-f") == 0) {
                                    f = 1;
                                } else if (strcmp(curr_argument, "-e") == 0) {
                                    e = 1;
                                } else if (strcmp(curr_argument, "-de") == 0 || strcmp(curr_argument, "-ed") == 0) {
                                    d = 1;
                                    e = 1;
                                } else if (strcmp(curr_argument, "-fe") == 0 || strcmp(curr_argument, "-ef") == 0) {
                                    f = 1;
                                    e = 1;
                                } else {
                                    // if any other flag is provided other than the seven mentioned
                                    if (ap == 1 || w == 1) {
                                        bprintf(global_buffer, "seek: Invalid Flag\n");
                                    }
                                    else {
                                        bprintf(global_buffer, "\033[1;31mseek: Invalid Flag\033[1;0m\n");
                                    }
                                    overall_success = 0;
                                    valid_flags = 0;
                                }
                            }
                        } else if (curr_argument[0] == '.') { // if path relative to current directory is provided
                            base_dir_flag = 1;
                            if (ip == 0) {
                                strcpy(base_dir, curr_argument);
                            } else if (ip == 1) {
                                char inp_buff[999999] = {0};

                                char file_path[MAX_LEN];
                                if (input_file_name_redirection[0] != '/') {
                                    strcpy(file_path, cwd);
                                    strcat(file_path, "/");
                                    strcat(file_path, input_file_name_redirection);
                                } else if (input_file_name_redirection[0] == '/') {
                                    strcpy(file_path, input_file_name_redirection);
                                }

                                int fd = open(file_path, O_RDONLY);
                                if (fd < 0) {
                                    perror("Error in opeaning the input file");
                                    overall_success = 0;
                                } else {
                                    int bytes_read = read(fd, inp_buff, 999998);
                                    if (bytes_read < 0) {
                                        perror("Error in reading");
                                        close(fd);
                                        overall_success = 0;
                                    } else {
                                        close(fd);
                                        if (inp_buff[strlen(inp_buff) - 1] == '\n') {
                                            inp_buff[strlen(inp_buff) - 1] = '\0';
                                        }
                                        strcpy(base_dir, inp_buff);
                                    }
                                }
                            }
                        } else {
                            if (file_name_flag == 1) {
                                if (ap == 1 || w == 1) {
                                    bprintf(global_buffer, "seek: invalid arguments\n");
                                }
                                else {
                                    bprintf(global_buffer, "\033[1;31mseek: invalid arguments\033[1;0m\n");
                                }
                                overall_success = 0;
                            } else {
                                file_name_flag = 1;
                                strcpy(file_name, curr_argument);
                            }
                        }
                    }
                }
                if (valid_flags == 1 && file_name_flag == 1) {
                    if (d == 1 && f == 1) {
                        // both d and f flags cannot be provided simultaneously
                        if (ap == 1 || w == 1) {
                            bprintf(global_buffer, "Invalid flags\n");
                        }
                        else {
                            bprintf(global_buffer, "\033[1;31mInvalid flags\033[1;0m\n");
                        }
                        overall_success = 0;
                    } else {
                        // iterating through all the files in the path
                        linked_list_head paths = create_linked_list_head();
                        char* path_to_base_dir;
                        if (base_dir_flag) {
                            path_to_base_dir = generate_new_path(cwd, base_dir, prev_dir, home_directory);
                        } else {
                            path_to_base_dir = (char*) calloc(MAX_LEN, sizeof(char));
                            strcpy(path_to_base_dir, cwd);
                        }

                        // seeking in each file/folder recursively
                        seek(path_to_base_dir, file_name, paths);

                        if (paths->number_of_nodes == 0) {
                            bprintf(global_buffer, "No match found!\n");
                        } else {
                            int no_of_files = 0;
                            int no_of_dir = 0;

                            linked_list_node temp = paths->first;
                            struct stat dir_stat_temp;
                            lstat(temp->path, &dir_stat_temp);
                            if (S_ISDIR(dir_stat_temp.st_mode) != 0) {
                                no_of_dir++;
                            } else {
                                no_of_files++;
                            }

                            if (e) {
                                if ((d && (no_of_dir > 1)) || (f && (no_of_files > 1))) {
                                    // do nothing
                                } else {
                                    if (d) {
                                        linked_list_node trav = paths->first;
                                        while (trav != NULL) {
                                            struct stat dir_stat;
                                            lstat(trav->path, &dir_stat);
                                            if (S_ISDIR(dir_stat.st_mode) != 0) { // checking if it's a dir
                                                if (dir_stat.st_mode & S_IXUSR) {
                                                    if (ap == 1 || w == 1) {
                                                        char buff[MAX_LEN] = {0};
                                                        sprintf(buff, "%s\n", relative_path(trav->path, path_to_base_dir));
                                                        bprintf(global_buffer, buff);
                                                    }
                                                    else {
                                                        char buff[MAX_LEN] = {0};
                                                        sprintf(global_buffer, "\033[1;34m%s\033[1;0m\n", relative_path(trav->path, path_to_base_dir));
                                                        bprintf(global_buffer, buff);
                                                    }
                                                    strcpy(prev_dir, cwd);
                                                    strcpy(cwd, trav->path);
                                                } else {
                                                    bprintf(global_buffer, "Missing permissions for task!\n");
                                                }
                                            }
                                            trav = trav->next;
                                        }
                                    } else if (f) {
                                        linked_list_node trav = paths->first;
                                        while (trav != NULL) {
                                            struct stat dir_stat;
                                            lstat(trav->path, &dir_stat);
                                            if (S_ISDIR(dir_stat.st_mode) == 0) { // checking if it's a file
                                                if (dir_stat.st_mode & S_IRUSR) {
                                                    if (ap == 1 || w == 1) {
                                                        char buff[MAX_LEN] = {0};
                                                        sprintf(buff, "%s\n", relative_path(trav->path, path_to_base_dir));
                                                        bprintf(global_buffer, buff);
                                                    }
                                                    else {
                                                        char buff[MAX_LEN] = {0};
                                                        sprintf(buff, "\033[1;32m%s\033[1;0m\n", relative_path(trav->path, path_to_base_dir));
                                                        bprintf(global_buffer, buff);
                                                    }
                                                    char buffer[100000];
                                                    FILE* fptr = fopen(trav->path, "r");
                                                    fgets(buffer, 100000, fptr);
                                                    fclose(fptr);
                                                    char buff[100001] = {0};
                                                    sprintf(buff, "%s\n", buffer);
                                                    bprintf(global_buffer, buff);
                                                } else {
                                                    bprintf(global_buffer, "Missing permissions for task!\n");
                                                }
                                            }
                                            trav = trav->next;
                                        }
                                    } else {
                                        if (paths->number_of_nodes > 1) {
                                            // do nothing
                                        } else {
                                            traverse_and_print(paths, 1, 1, path_to_base_dir, ap, w);
                                        }
                                    }
                                }
                            } else {
                                if (f) {
                                    traverse_and_print(paths, 1, 0, path_to_base_dir, ap, w);
                                } else if (d) {
                                    traverse_and_print(paths, 0, 1, path_to_base_dir, ap, w);
                                } else {
                                    traverse_and_print(paths, 1, 1, path_to_base_dir, ap, w);
                                }
                            }
                        }
                        free(path_to_base_dir);
                        free_linked_list(paths);
                    }
                }

                free(base_dir);
                free(file_name);

                io_redirection(ap, w, cwd, output_file_name_redirection);
                
                if (success == 0) overall_success = 0;
            }
    // ===================================================================================
            // exit

            // checking if exit command is present
            else if (strcmp("exit", argument_tokens[0]) == 0) {
                if (no_of_arguments > 1) {
                    printf("\033[1;31mInvalid command\033[1;0m\n");
                } else {
                    LL_Node trav = LL->first;
                    while (trav != NULL) {
                        kill(trav->pid, SIGKILL);
                        trav = trav->next;
                    }
                    store_command(curr_command, home_directory);
                    exit(0);
                }
            }
    // ===================================================================================
            // activities

            // checking if activities command is present
            else if (strcmp("activities", argument_tokens[0]) == 0) {
                print_active_processes_spawned_by_my_shell();

                io_redirection(ap, w, cwd, output_file_name_redirection);
            }
    // ===================================================================================
            // ping

            // checking if ping command is present
            else if (strcmp("ping", argument_tokens[0]) == 0) {
                if (no_of_arguments < 2) {
                    if (ap == 1 || w == 1) {
                        bprintf(global_buffer, "ping: Invalid Arguments\n");
                    } else {
                        bprintf(global_buffer, "\033[1;31mping: Invalid Arguments\033[1;0m\n");
                    }
                } else {
                    int pid = atoi(argument_tokens[1]);
                    int sig = atoi(argument_tokens[2]);

                    int exit_status = ping_sig(pid, sig, ap, w);
                    if (exit_status == 0) overall_success = 0;
                }

                io_redirection(ap, w, cwd, output_file_name_redirection);
            }
    // ===================================================================================
            // bg

            // checking if bg command is present
            else if (strcmp("bg", argument_tokens[0]) == 0) {
                int pid = atoi(argument_tokens[1]);

                int result = kill(pid, 0);
                if (result == 0) {
                    kill(pid, SIGCONT);
                } else {
                    printf("\033[1;31mNo such process exists\033[1;0m\n");
                    overall_success = 0;
                }
                io_redirection(ap, w, cwd, output_file_name_redirection);
            }
    // ===================================================================================
            // fg

            // checking if fg command is present
            else if (strcmp("fg", argument_tokens[0]) == 0) {
                int pid = atoi(argument_tokens[1]);

                int result = kill(pid, 0);

                if (result == 0) {
                    int cstatus;
                    kill(pid, SIGCONT);
                    waitpid(pid, &cstatus, WUNTRACED);
                } else {
                    printf("\033[1;31mNo such process exists\033[1;0m\n");
                    overall_success = 0;
                }

                io_redirection(ap, w, cwd, output_file_name_redirection);
            }
    // ===================================================================================
            // system commands

            else {
                if (strcmp(argument_tokens[0], "echo") == 0) {
                    for (int i = 1; i <= no_of_arguments; i++) {
                        if (argument_tokens[i] != NULL) {
                            if (argument_tokens[i][0] == '"') {
                                for (int j = 0; j < strlen(argument_tokens[i]) - 1; j++) {
                                    argument_tokens[i][j] = argument_tokens[i][j + 1];
                                }
                                argument_tokens[i][strlen(argument_tokens[i]) - 1] = '\0';
                            } 
                            if (argument_tokens[i][strlen(argument_tokens[i]) - 1] == '"') {
                                argument_tokens[i][strlen(argument_tokens[i]) - 1] = '\0';
                            }
                        } else {
                            break;
                        }
                    }
                }

                bg_process = 0;

                if (curr_command[strlen(curr_command) - 1] == '&') {
                    bg_process = 1;
                    argument_tokens[no_of_arguments][strlen(argument_tokens[no_of_arguments]) - 1] = '\0';
                } else {
                    bg_process = 0;
                }

                int ppid = getpid();
                int pid = fork();

                if (pid == 0) {
                    if (bg_process == 1) {
                        setpgid(0, 0);
                    }
                    // creating absolute path to the file (input)
                    char inp_file_path[MAX_LEN];
                    if (input_file_name_redirection == NULL) {
                        // do nothing
                    } else if (input_file_name_redirection[0] != '/') {
                        strcpy(inp_file_path, cwd);
                        strcat(inp_file_path, "/");
                        strcat(inp_file_path, input_file_name_redirection);
                    } else if (input_file_name_redirection[0] == '/') {
                        strcpy(inp_file_path, input_file_name_redirection);
                    }

                    // creating absolute path to the file (output)
                    char out_file_path[MAX_LEN];
                    if (output_file_name_redirection == NULL) {
                        // do nothing
                    } else if (output_file_name_redirection[0] != '/') {
                        strcpy(out_file_path, cwd);
                        strcat(out_file_path, "/");
                        strcat(out_file_path, output_file_name_redirection);
                    } else if (output_file_name_redirection[0] == '/') {
                        strcpy(out_file_path, output_file_name_redirection);
                    }

                    if (w == 1 && ip == 1) {
                        close(STDOUT_FILENO);
                        open(out_file_path, O_CREAT | O_WRONLY | O_TRUNC, 0644);

                        int inp_fd = open(inp_file_path, O_RDONLY);
                        if (inp_fd < 0) {
                            // open failed
                            printf("\033[1;31mopen : %s\033[1;0m\n", strerror(errno));
                            // killing child process
                            kill(getpid(), SIGTERM);
                        } else {
                            if (dup2(inp_fd, STDIN_FILENO) == -1) {
                                // dup2 failed
                                printf("\033[1;31mdup2 : %s\033[1;0m\n", strerror(errno));
                                // closing the opened file
                                close(inp_fd);
                                // killing child process
                                kill(getpid(), SIGTERM);
                            } else {
                                close(inp_fd);
                                execvp(argument_tokens[0],  argument_tokens);
                                // execvp failed
                                printf("\033[1;31mexecvp : %s\033[1;0m\n", strerror(errno));
                                kill(getpid(), SIGTERM);
                            }
                        }
                    } if (w == 1) {
                        close(STDOUT_FILENO);
                        open(out_file_path, O_CREAT | O_WRONLY | O_TRUNC, 0644);
                        execvp(argument_tokens[0], argument_tokens);
                        // error
                        printf("\033[1;31m%s : %s\033[1;0m\n", argument_tokens[0], strerror(errno));
                        kill(getpid(), SIGTERM);
                    } else if (ap == 1) {
                        
                        close(STDOUT_FILENO);
                        open(out_file_path, O_CREAT | O_WRONLY | O_APPEND, 0644);
                        execvp(argument_tokens[0], argument_tokens);
                        // error
                        printf("\033[1;31m%s : %s\033[1;0m\n", argument_tokens[0], strerror(errno));
                        kill(getpid(), SIGTERM);

                    } else if (ip == 1) {
                        int inp_fd = open(inp_file_path, O_RDONLY);
                        if (inp_fd < 0) {
                            // open failed
                            printf("\033[1;31mopen : %s\033[1;0m\n", strerror(errno));
                            kill(getpid(), SIGTERM);
                        } else {
                            if (dup2(inp_fd, STDIN_FILENO) == -1) {
                                // dup2 failed
                                printf("\033[1;31mdup2 : %s\033[1;0m\n", strerror(errno));
                                close(inp_fd);
                                kill(getpid(), SIGTERM);
                            } else {
                                close(inp_fd);
                                execvp(argument_tokens[0],  argument_tokens);
                                // execvp failed
                                printf("\033[1;31mexecvp : %s\033[1;0m\n", strerror(errno));
                                kill(getpid(), SIGTERM);
                            }
                        }
                    } else {
                        execvp(argument_tokens[0], argument_tokens);
                        // execvp failed
                        printf("\033[1;31m%s : %s\033[1;0m\n", argument_tokens[0], strerror(errno));
                        kill(getpid(), SIGTERM);
                    }
                } else if (pid > 0) {
                    if (bg_process == 0) {
                        int cstatus;
                        global_fg_pid = pid;
                        char* temp = (char*) calloc(MAX_LEN, sizeof(char));
                        strcpy(temp, curr_command);
                        fg_command_name = temp;
                        waitpid(pid, &cstatus, WUNTRACED);
                        global_fg_pid = -1;
                        free(fg_command_name);
                        fg_command_name = NULL;
                    } else {
                        printf("%d\n", pid);
                        insert_in_LL(pid, -1, argument_tokens);
                    }
                } else {
                    if (w == 1 || ap == 1) {
                        bprintf(global_buffer, "fork: could not fork\n");
                    } else {
                        bprintf(global_buffer, "\033[1;31mfork: could not fork\033[1;0m\n");
                    }
                    overall_success = 0;
                }
            }
    // ===================================================================================
            free_tokens(argument_tokens);
        }
        idx++;
        curr_command = list_of_commands[idx];
    }
    if (store == 1 && pastevents_present == 0 && overall_success == 1) {
        store_command(input_string, home_directory);
    }

    free(input_string);
    free_commands_list(list_of_commands);
}