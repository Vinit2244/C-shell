#include "headers.h"

int bg_process = 0; // Flag to mark if a process is background process or not
int start = 0;      // Variable to hold the start time of command execution
time_t tyme = 0;    // Variable to hold the time of execution

void input(char* command, char* home_directory, char* cwd, char* prev_dir, int store, char* last_command, int* t, int w, int ap, char* file_name_redirection) {

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

        int append_flag = 0;
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

        int write_flag = 0;
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
            // int idd = 0;
            // while (sub_commands[idd] != NULL) {
            //     printf("%s\n", sub_commands[idd]);
            //     printf("%d\n", strlen(sub_commands[idd]));
            //     idd++;
            // }

            int k = 0;
            while (sub_commands[k] != NULL) {
                input(sub_commands[k], home_directory, cwd, prev_dir, store, last_command, t, 0, 0, NULL);
                k++;
            }
            free_tokens(sub_commands);
            // return;
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

            input(app_cmd, home_directory, cwd, prev_dir, store, last_command, t, 0, 1, to_file);
            free(app_cmd);
            free(to_file);
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
            input(cmd, home_directory, cwd, prev_dir, store, last_command, t, 1, 0, output_file);
            free_tokens(params);
        } else if (inp_flag == 1) {
            char** files = generate_tokens(curr_command, '<');
            // int idd = 0;
            // while (files[idd] != NULL) {
            //     printf("%s\n", files[idd]);
            //     printf("%d\n", strlen(files[idd]));
            //     idd++;
            // }

            free_tokens(files);
            // return;
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

            // IO redirection remaining
            if (strcmp("iMan", argument_tokens[0]) == 0) {
                // DNS resolution
                char hostname[] = "man.he.net";
                struct addrinfo specs;

                memset(&specs, 0, sizeof(specs)); // initializing to 0

                specs.ai_family = AF_UNSPEC;     // Address family unspecified (return both IPv4 and IPv6 addresses)
                specs.ai_socktype = SOCK_STREAM; // TCP

                struct addrinfo *LL_of_addresses;
                // resolving DNS
                int status = getaddrinfo(hostname, NULL, &specs, &LL_of_addresses); // returns a linked list of struct addrinfo stored at position LL_of_addresses
                if (status != 0) {
                    perror(gai_strerror(status)); // gai_strerror() converts the error code returned by the getaddrinfo() function in string form
                    overall_success = 0;
                } else {
                    void *addr; // stores binary IP address
                    char ip[MAX_LEN];

                    // Iterating through all the addresses returned
                    struct addrinfo *trav = LL_of_addresses;
                    while (trav != NULL) {
                        // trav->ai_addr is a generic structure that can hold information about both ipv4 and ipv6 address so first we need to check which address are we talking about and then accordingly resolve the sockaddr_in struct
                        if (trav->ai_family == AF_INET) { // IPv4
                            struct sockaddr_in *ipv4 = (struct sockaddr_in *) trav->ai_addr;
                            addr = &(ipv4->sin_addr);
                        } else { // IPv6 (not using IPv6 only using IPv4)
                            trav = trav->ai_next;
                            continue;
                        }
                        // Converting IP address to string
                        inet_ntop(trav->ai_family, addr, ip, MAX_LEN);
                        trav = trav->ai_next;
                    }
                    freeaddrinfo(LL_of_addresses);

                    // Opeaning a TCP socket
                    int port = 80; // HTTP
                    int sock;
                    struct sockaddr_in address;
                    socklen_t address_size;

                    sock = socket(AF_INET, SOCK_STREAM, 0);
                    if (sock < 0) {
                        perror("Socket error");
                        overall_success = 0;
                    } else {
                        memset(&address, 0, sizeof(address));
                        address.sin_family = AF_INET;
                        address.sin_port = port;
                        address.sin_addr.s_addr = inet_addr(ip);

                        // Connecting
                        int status = connect(sock, (struct sockaddr*) &address, sizeof(address));
                        if (status == -1) {
                            perror("Failed connecting");
                            overall_success = 0;
                        } else {
                            // Sending Get request
                            char GET_request[MAX_LEN];
                            sprintf(GET_request, "GET %s/%s HTTP/1.1\r\nHost: %s\r\n\r\n", "/man1", argument_tokens[1], hostname);
                            
                            // if sending GET request fails
                            if (send(sock, GET_request, strlen(GET_request), 0) == -1) {
                                perror("Request failed");
                                overall_success = 0;
                                close(sock);
                            } else {
                                char buffer[999999];
                                ssize_t bytes_received;

                                while ((bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
                                    buffer[bytes_received] = '\0';
                                    printf("%s", buffer);
                                }

                                // closing the socket
                                close(sock);
                            }
                        }
                    }
                }
            }
    // ===================================================================================
            // warp
            /*
                Changes the current working directory to the relative/absolute path provided
            */

            // checking if warp command is present
            else if (strcmp("warp", argument_tokens[0]) == 0) {
                int success = 1;            // flag to keep track if warp exited successfully or not
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

                io_redirection(ap, w, cwd, file_name_redirection);

                if (success == 0) overall_success = 0;
            }
    // ===================================================================================
            // peek
            /*
                Prints the files/directories present in the path of file provided
            */

            // checking if peek command is present
            else if (strcmp("peek", argument_tokens[0]) == 0) {
                char path[MAX_LEN]; // stores path of directory to peek into
                
                // copying cwd into path
                for (int i = 0; i < strlen(cwd); i++) {
                    path[i] = cwd[i];
                }
                path[strlen(cwd)] = '\0';

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

                io_redirection(ap, w, cwd, file_name_redirection);
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
                        // clears the stored list
                        purge(ap, w, home_directory);
                    } else if (strcmp(argument_tokens[1], "execute") == 0) { // execute some pastevent whose event is given
                        if (no_of_arguments == 1) {
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
                        } else if (no_of_arguments == 2) {
                            char* number = argument_tokens[2];
                            int num = 0;    // variable to store the index of command to execute
                            int flag = 1;   // valid index is passed between 1 and 15 (both inclusive)
                            
                            // converting char to int
                            // doing it the long way since some invalid string can also be given in argument so i had to handle it manually and not use atoi()
                            if (strcmp(number, "1") == 0) {
                                num = 1;
                            } else if (strcmp(number, "2") == 0) {
                                num = 2;
                            } else if (strcmp(number, "3") == 0) {
                                num = 3;
                            } else if (strcmp(number, "4") == 0) {
                                num = 4;
                            } else if (strcmp(number, "5") == 0) {
                                num = 5;
                            } else if (strcmp(number, "6") == 0) {
                                num = 6;
                            } else if (strcmp(number, "7") == 0) {
                                num = 7;
                            } else if (strcmp(number, "8") == 0) {
                                num = 8;
                            } else if (strcmp(number, "9") == 0) {
                                num = 9;
                            } else if (strcmp(number, "10") == 0) {
                                num = 10;
                            } else if (strcmp(number, "11") == 0) {
                                num = 11;
                            } else if (strcmp(number, "12") == 0) {
                                num = 12;
                            } else if (strcmp(number, "13") == 0) {
                                num = 13;
                            } else if (strcmp(number, "14") == 0) {
                                num = 14;
                            } else if (strcmp(number, "15") == 0) {
                                num = 15;
                            } else {
                                flag = 0;
                                if (ap == 0 && w == 0) {
                                    bprintf(global_buffer, "\033[1;31mpastevents: argument value should a positive integer less than or equal to 15\033[1;0m\n");
                                } else {
                                    bprintf(global_buffer, "pastevents: argument value should a positive integer less than or equal to 15\n");
                                }
                            }
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
                io_redirection(ap, w, cwd, file_name_redirection);
            }
    // ===================================================================================
            // proclore
            /*
                Prints all the details about the process whose process id is passed as an argument
            */

            // checking if proclore command is present
            else if (strcmp("proclore", argument_tokens[0]) == 0) {
                char* pid = (char*) calloc(50, sizeof(char));
                if (no_of_arguments == 0) {         // if no arguments are passed than print the details of the termianl process
                    int curr_pid = getpid();        // getting the pid of the current running process
                    sprintf(pid, "%d", curr_pid);   // writing the pid in the form of a string to pass
                } else { // if pid of the process is provided
                    strcpy(pid, argument_tokens[1]);
                }
                proclore(pid, ap, w);

                io_redirection(ap, w, cwd, file_name_redirection);

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
                                } else {
                                    // if any other flag is provided other than the three mentioned
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
                            strcpy(base_dir, curr_argument);
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
                                                    char buff[MAX_LEN] = {0};
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

                io_redirection(ap, w, cwd, file_name_redirection);
                
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
            // system commands
            
            else {
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
                    setpgid(0, 0);
                    if (w == 1) {
                        char file_path[MAX_LEN];
                        strcpy(file_path, cwd);
                        strcat(file_path, "/");
                        strcat(file_path, file_name_redirection);

                        close(STDOUT_FILENO);
                        open(file_path, O_CREAT | O_WRONLY | O_TRUNC, 0644);
                        execvp(argument_tokens[0], argument_tokens);
                    } else if (ap == 1) {
                        char file_path[MAX_LEN];
                        strcpy(file_path, cwd);
                        strcat(file_path, "/");
                        strcat(file_path, file_name_redirection);
                        
                        close(STDOUT_FILENO);
                        open(file_path, O_CREAT | O_WRONLY | O_APPEND, 0644);
                        execvp(argument_tokens[0], argument_tokens);
                    } else {
                        execvp(argument_tokens[0], argument_tokens);
                        perror(argument_tokens[0]);
                        
                    }
                } else if (pid > 0) {
                    if (bg_process == 0) {
                        wait(NULL);
                    } else {
                        printf("%d\n", pid);
                        insert_in_LL(pid, -1);
                    }
                } else {
                    printf("\033[1;31mfork: could not fork\033[1;0m\n");
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