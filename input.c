#include "headers.h"

int bg_process = 0; // Flag to mark if a process is background process or not
int start = 0;      // Variable to hold the start time of command execution
time_t tyme = 0;    // Variable to hold the time of execution

int global_fg_pid;  // Stores the pid of the currently running foreground process

void input(char* command, int store, int w, int ap, int ip, char* output_file_name_redirection, char* input_file_name_redirection) {

    global_fg_pid = -1;         // -1 represents no foreground process is initiated by my terminal

    tyme = time(NULL) - start;  // current time - start time
    t = tyme;                   // global variable to hold the time of last executed command

    start = 0;                  // reset start time to zero
    tyme = 0;                   // reset execution time to zero

    int overall_success = 1;    // flag to mark if the complete command ran without failing
    int pastevents_present = 0; // flag to mark if pastevents command is present
    int pastevents_execute_present = 0;

    char input_string[5000] = {0};
    
    if (command == NULL) {  // command is NULL if the input function is called from main and not from pastevents execute function
        store = 1;          // flag to reflect if the command inputted is to be stored or not

        // Print appropriate prompt with username, systemname and directory before accepting input
        int exit_status = prompt();
        if (exit_status == 0) {
            fprintf(stderr, "\033[1;31mprompt: Could Not Print prompt\033[1;0m\n");
            return;
        }
        
        // when we press ctrl + d fgets returns NULL
        if (fgets(input_string, 4096, stdin) == NULL) {
            handle_ctrl_d();
        }
        input_string[strlen(input_string)] = '\0';

        // removing endline character
        if (input_string[strlen(input_string) - 1] == '\n') input_string[strlen(input_string) - 1] = '\0';

        // check through the linked list of all the background processes that were running and print the ones which are done
        check_and_print();
        
        if (bg_process_buffer[0] != '\0') {
            printf("%s", bg_process_buffer);
            bg_process_buffer[0] = '\0';
        }

    } else {        // input is called by pastevents execute
        store = 0;  // don't store the command as it was called by pastevents execute
        strcpy(input_string, command);
    }

    last_command[0] = '\0';

    char** list_of_commands = get_list_of_commands(input_string); // contains the list of all commands as separate strings in the form of a 2D array

    start = time(NULL); // record the time before we start executing the commands

    int idx = 0;        // index variable to hold the index of command to be executed
    char* curr_command = list_of_commands[idx];
    while (curr_command[0] != '\0') {
        // Checking if the command involves piping
        int pipe_flag = is_pipe_present(curr_command);

        // output redirection '>>'
        char* app_cmd = (char*) calloc(MAX_LEN, sizeof(char));
        char* to_file = (char*) calloc(MAX_LEN, sizeof(char));
        int append_flag = is_append_present(curr_command, app_cmd, to_file);

        // output redirection '>'
        int write_flag = is_write_present(curr_command);
        
        // input redirection '<'
        int inp_flag = is_input_present(curr_command);

        if (pipe_flag == 1) {
            // int res = is_pastevents_execute_present(curr_command);

            char** list_of_commands_pipe = generate_tokens(curr_command, '|');
            int num_of_commands = 0;
            while (list_of_commands_pipe[num_of_commands] != NULL) {
                num_of_commands++;
            }

            int num_pipes = num_of_commands - 1;

            // 0 - read
            // 1 - write
            int pipe_fd[num_pipes][2];
            for (int i = 0; i < num_pipes; i++) {
                if (pipe(pipe_fd[i]) < 0) {
                    fprintf(stderr, "033[1;31mError occured while piping\033[1;0m\n");
                    return;
                }
            }

            for (int i = 0; i < num_of_commands; i++) {
                // running each sub command
                int pid_i = fork();
                if (pid_i < 0) {
                    fprintf(stderr, "033[1;31merror while fork\033[1;0m\n");
                    return;
                } else if (pid_i == 0) {
                    char** argument_tokens = generate_tokens(list_of_commands_pipe[i], ' ');
                    if (i == 0) {
                        for (int j = 1; j < num_pipes; j++) {
                            close(pipe_fd[j][0]);
                            close(pipe_fd[j][1]);
                        }

                        close(pipe_fd[0][0]);
                        dup2(pipe_fd[0][1], STDOUT_FILENO);
                        close(pipe_fd[0][1]);
                    } else if (i > 0 && i < num_of_commands - 1) {
                        for (int j = 0; j < num_pipes; j++) {
                            if (j == i - 1 || j == i) continue;
                            close(pipe_fd[j][0]);
                            close(pipe_fd[j][1]);
                        }

                        // pipe number to take input = i - 1
                        close(pipe_fd[i - 1][1]); // closing the write end of pipe i - 1
                        // pipe number to write output = i
                        close(pipe_fd[i][0]);    // closing the read end of pipe i

                        dup2(pipe_fd[i - 1][0], STDIN_FILENO);
                        dup2(pipe_fd[i][1], STDOUT_FILENO);

                        close(pipe_fd[i - 1][0]);
                        close(pipe_fd[i][1]);
                    } else if (i == num_of_commands - 1) {
                        for (int j = 0; j < num_pipes - 1; j++) {
                            close(pipe_fd[j][0]);
                            close(pipe_fd[j][1]);
                        }

                        close(pipe_fd[num_pipes - 1][1]);
                        dup2(pipe_fd[num_pipes - 1][0], STDIN_FILENO);
                        close(pipe_fd[num_pipes - 1][0]);
                    }
                    execvp(argument_tokens[0], argument_tokens);
                    fprintf(stderr, "033[1;31merror: execvp\033[1;0m\n");
                    kill(getpid(), SIGTERM);
                }
            }

            for (int i = 0; i < num_pipes; i++) {
                close(pipe_fd[i][0]);
                close(pipe_fd[i][1]);
            }
            for (int i = 0; i < num_of_commands; i++) {
                // closing all pipes in parent execept the read of last pipe
                wait(NULL);
            }

            free_tokens(list_of_commands_pipe);
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
                input(cmd, store, 0, 0, 1, NULL, inp_file);

                free_tokens(files);
            } else {
                if (write_flag == 1) {
                    // assuming of the form cmd < input_file > output_file
                    char** tkns = generate_tokens(curr_command, '<');
                    char* cmd = tkns[0];

                    char** tkns2 = generate_tokens(tkns[1], '>');
                    char* input_file = tkns2[0];
                    char* output_file = tkns2[1];

                    input(cmd, store, 1, 0, 1, output_file, input_file);

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
            input(cmd, store, 1, 0, 0, output_file, NULL);
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

            input(app_cmd, store, 0, 1, 0, to_file, NULL);
            free(app_cmd);
            free(to_file);
        } else {
            // tokenize the command based on strings
            char** argument_tokens = generate_tokens(curr_command, ' ');

            // calculating the number of arguments passed to the command
            int no_of_arguments = 0;
            while(argument_tokens[no_of_arguments] != NULL) {
                no_of_arguments++;
            }
            // Number of arguments is one less than the total number of tokens because,
            // the first token will be the command name and rest will be arguments
            no_of_arguments--;

            // Different commands
    // ===================================================================================
            // iMan
            /*
                DESCRIPTION: 
                    Reads the Man pages of the specified command from man.he.net website and prints it
                
                SYNTAX : 
                    iMan <name of command>
            */

            // checking if iMan command is present
            if (strcmp("iMan", argument_tokens[0]) == 0) {
                int exit_status = iMan(argument_tokens, no_of_arguments, ip, input_file_name_redirection);
                
                int success = io_redirection(ap, w, cwd, output_file_name_redirection);

                if (exit_status == 0 || success == 0) overall_success = 0;
            }
    // ===================================================================================
            // warp
            /*
                DESCRIPTION: 
                    Changes the current working directory to the relative/absolute path provided
                
                SYNTAX: 
                    warp [<relative/absolute path to directory>, ...]
            */

            // checking if warp command is present
            else if (strcmp("warp", argument_tokens[0]) == 0) {
                int exit_status = change_cwd(w, ap, ip, argument_tokens, no_of_arguments);

                int success = io_redirection(ap, w, cwd, output_file_name_redirection);

                if (exit_status == 0 || success == 0) overall_success = 0;
            }
    // ===================================================================================
            // peek
            /*
                DESCRIPTION: 
                    Prints the files/directories present in the path of file provided
                    flags: -l: Displays extra information
                           -a: Displays all files, including hidden files
                
                SYNTAX: peek <flags> <path/name>
            */

            // checking if peek command is present
            else if (strcmp("peek", argument_tokens[0]) == 0) {
                int exit_status = look_into_file(argument_tokens, no_of_arguments, ap, w);

                int success = io_redirection(ap, w, cwd, output_file_name_redirection);

                if (exit_status == 0 || success == 0) overall_success = 0;
            }
    // ===================================================================================
            // pastevents
            /*
            DESCRIPTION:
                pastevents             : Prints the last 15 commands executed
                pastevents purge       : Clears History
                pastevents execute <i> : Executes the last ith command executed (1 <= i <= 15)
            
            SYNTAX: 
                pastevents
                pastevents purge
                pastevents execute <int>
            */

            // checking if pastevents command is present
            else if (strcmp("pastevents", argument_tokens[0]) == 0) {
                int exit_status = history(argument_tokens, no_of_arguments, &pastevents_present, ap, w, ip, curr_command, input_file_name_redirection, store, &pastevents_execute_present, input_string);
                
                int success = io_redirection(ap, w, cwd, output_file_name_redirection);

                if (exit_status == 0 || success == 0) overall_success = 0;
            }
    // ===================================================================================
            // proclore
            /*
                DESCRIPTION: 
                    Prints all the details about the process whose process id is passed as an argument. 
                    If no pid is provided it prints the details about the current process (shell)

                SYNTAX: 
                    proclore <pid>
            */

            // checking if proclore command is present
            else if (strcmp("proclore", argument_tokens[0]) == 0) {
                int exit_status = print_info_pid(argument_tokens, no_of_arguments, w, ap, ip, input_file_name_redirection);

                int success = io_redirection(ap, w, cwd, output_file_name_redirection);

                if (exit_status == 0 || success == 0) overall_success = 0;
            }
    // ===================================================================================
            // seek
            /*
                DESCRIPTION:
                    Searches for a file/directory in the relative/absolute path provided
                    Searches the entire depth of the tree starting from the provided path
                    flags: -e: Checks if only one directory/file with the given name is found
                            (if file is found      : prints it's outpus)
                            (if directory is found : changes cwd to that directory)
                           -f: Checks only for files of specified name
                           -d: Checks only for directories of specified name
                    If no relative/absolute path to the base directory is provided it just searches in the cwd

                SYNTAX: 
                    seek <flags> <name of file to be searched> [<relative/absolute path to base directory>]
            */

            // checking if seek command is present
            else if (strcmp("seek", argument_tokens[0]) == 0) {
                int exit_status = find_file_dir(argument_tokens, no_of_arguments, w, ap, ip, input_file_name_redirection);

                int success = io_redirection(ap, w, cwd, output_file_name_redirection);
                
                if (success == 0 || exit_status == 0) overall_success = 0;
            }
    // ===================================================================================
            // exit
            /*
                DESCRIPTION:
                    Exits the shell after ending all the running background processes spawned by the shell

                SYNTAX:
                    exit
            */

            // checking if exit command is present
            else if (strcmp("exit", argument_tokens[0]) == 0) {
                if (no_of_arguments > 1) {
                    fprintf(stderr, "\033[1;31mInvalid command\033[1;0m\n");
                } else {
                    LL_Node trav = LL->first;
                    while (trav != NULL) {
                        kill(trav->pid, SIGKILL);
                        trav = trav->next;
                    }
                    if (store_command(curr_command) == 1) {
                        exit(0);
                    } else {
                        fprintf(stderr, "\033[1;31mexit : Error occured while storing the command\033[1;0m\\n");
                        return;
                    }
                }
            }
    // ===================================================================================
            // activities
            /*
                DESCRIPTION:
                    Prints the list of all the background processes spawned by my shell which is currently
                    running or stopped in lexicographical order of pid
                
                SYNTAX:
                    activities
            */

            // checking if activities command is present
            else if (strcmp("activities", argument_tokens[0]) == 0) {
                int exit_status = print_active_processes_spawned_by_my_shell();

                int success = io_redirection(ap, w, cwd, output_file_name_redirection);

                if (exit_status == 0 || success == 0) overall_success = 0;
            }
    // ===================================================================================
            // ping
            /*
                DESCRIPTION:
                    Sends given signal signal to the process with the given pid

                SYNTAX:
                    ping <pid> <signal number>
            */

            // checking if ping command is present
            else if (strcmp("ping", argument_tokens[0]) == 0) {
                int exit_status = ping(argument_tokens, no_of_arguments, w, ap);

                int success = io_redirection(ap, w, cwd, output_file_name_redirection);

                if (exit_status == 0 || success == 0) overall_success = 0;
            }
    // ===================================================================================
            // bg
            /*
                DESCRIPTION:
                    Changes the state of the background process whose pid is given to running if 
                    it is blocked else leaves it as it is
                
                SYNTAX:
                    bg <pid>
            */

            // checking if bg command is present
            else if (strcmp("bg", argument_tokens[0]) == 0) {
                int exit_status = run_bg_process(argument_tokens, no_of_arguments);
                
                int success = io_redirection(ap, w, cwd, output_file_name_redirection);

                if (exit_status == 0 || success == 0) overall_success = 0;
            }
    // ===================================================================================
            // fg
            /*
                DESCRIPTION:
                    Brings the background process with the given pid to foreground
                
                SYNTAX:
                    fg <pid>
            */

            // checking if fg command is present
            else if (strcmp("fg", argument_tokens[0]) == 0) {
                int exit_status = bring_process_to_foreground(argument_tokens);

                int success = io_redirection(ap, w, cwd, output_file_name_redirection);

                if (exit_status == 0 || success == 0) overall_success = 0;
            }
    // ===================================================================================
            // neonate
            /*
                DESCRIPTION:
                    Prints the pid of the most recently created process in the system at regular
                    time intervals (provided as argument) and exits on pressing 'x' on keyboard
                
                SYNTAX:
                    neonate -n <int>
            */

            //checking if neonate is present
            else if (strcmp("neonate", argument_tokens[0]) == 0) {
                int exit_status = print_pid_of_latest_process_in_interval(argument_tokens);
                
                int success = io_redirection(ap, w, cwd, output_file_name_redirection);

                if (exit_status == 0 || success == 0) overall_success = 0;
            }
    // ===================================================================================
            // system commands
            /*
                DESCRIPTION:
                    Runs all other system commands using execvp
            */

            else {
                overall_success = system_command(argument_tokens, no_of_arguments, curr_command, w, ap, ip, input_file_name_redirection, output_file_name_redirection);
            }
    // ===================================================================================
            if (overall_success == 1) {
                // storing the name of commands in the last command array
                if (strlen(last_command) == 0) {
                    if (strcmp(argument_tokens[0], "pastevents") != 0) {
                        strcpy(last_command, argument_tokens[0]);
                    }
                } else {
                    if (strcmp(argument_tokens[0], "pastevents") != 0) {
                        strcat(last_command, ", ");
                        strcat(last_command, argument_tokens[0]);
                    }
                }
            }
            free_tokens(argument_tokens);
        }
        idx++;
        curr_command = list_of_commands[idx];
    }

    if (store == 0 || (pastevents_present == 1 && pastevents_execute_present == 0) || overall_success == 0) {
        // do not store
    } else {
        if (store_command(input_string) == 0) {
            overall_success = 0;
            return;
        }
    }

    free_commands_list(list_of_commands);
}