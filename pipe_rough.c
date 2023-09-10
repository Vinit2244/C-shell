#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_LEN 100000

void remove_leading_and_trailing_spaces(char* str) {
    // removing leading spaces 
    int front_index = 0;
    while (str[front_index] == ' ' || str[front_index] == '\t') {
        front_index++;
    }
    int rear_index = strlen(str) - 1;
    while (str[rear_index] == ' ' || str[rear_index] == '\t') {
        rear_index--;
    }
    if (rear_index < front_index) {
        str[0] = '\0';
        return;
    }
    int len = rear_index - front_index + 1;
    char* final = (char*) malloc(sizeof(char) * (len + 1));
    for (int i = 0; i < len; i++) {
        final[i] = str[front_index++];
    }
    final[len] = '\0';
    for (int i = 0; i < len + 1; i++) {
        str[i] = final[i];
    }
    free(final);
}

char** generate_tokens(char* str, char c) {
    int no_of_characters = 0;
    int idx = 0;
    while (str[idx] != '\0') {
        if (str[idx] == c) no_of_characters++;
        idx++;
    }
    int no_of_partitions = no_of_characters + 1;
    char** tokens_array = (char**) calloc(no_of_partitions + 1, sizeof(char*));
    for (int i = 0; i < no_of_partitions; i++) {
        tokens_array[i] = (char*) calloc(MAX_LEN, sizeof(char));
    }
    tokens_array[no_of_partitions] = NULL;
    int str_idx = 0;
    int tokens_array_idx = 0;
    int token_idx = 0;
    while (str[str_idx] != '\0') {
        if (str[str_idx] == c) {
            tokens_array[tokens_array_idx][token_idx] = '\0';
            token_idx = 0;
            tokens_array_idx++;
        } else {
            tokens_array[tokens_array_idx][token_idx] = str[str_idx];
            token_idx++;
        }
        str_idx++;
    }

    tokens_array[tokens_array_idx][token_idx] = '\0';

    // Removing all the leading and trailing spaces from the commands if any present
    for (int i = 0; i <= tokens_array_idx; i++) {
        remove_leading_and_trailing_spaces(tokens_array[i]);
    }
    return tokens_array;
}

void free_tokens(char** tokens) {
    int idx = 0;
    while (tokens[idx] != NULL) {
        free(tokens[idx]);
        idx++;
    }
    free(tokens);
}

int main() {
    char* input_string = (char*) calloc(5000, sizeof(char));
    input_string[0] = '\0';

    printf("Enter input: ");

    fgets(input_string, 4096, stdin);
    input_string[strlen(input_string)] = '\0';

    // removing endline character
    for (int i = 0; i < strlen(input_string); i++) {
        if (input_string[i] != '\n') continue;
        else {
            input_string[i] = '\0';
        }
    }

    char** list_of_commands = generate_tokens(input_string, '|');
    int k = 0;
    while (list_of_commands[k] != NULL) {
        printf("%s\n", list_of_commands[k]);
        k++;
    }

    printf("Number of commands passed = %d\n", k);

    // 0 - read
    // 1 - write
    int fd[k][2];
    for (int i = 0; i < k; i++) {
        if (pipe(fd[i]) < 0) {
            printf("Error occured while piping\n");
            return 1;
        }
    }

    for (int i = 0; i < k; i++) {
        char** argument_tokens = generate_tokens(list_of_commands[0], ' ');
        int pid = fork();

        if (pid < 0) {
            printf("Error occured in fork\n");
            return 1;
        } else if (pid == 0) {
            for (int j = 0; j < k; j++) {
                if (j == i) continue;
                else {
                    close(fd[j][0]);
                    close(fd[j][1]);
                }
            }
            if (i == 0) { // only writes to the pipe
                close(fd[i][0]);
                dup2(fd[i][1], STDOUT_FILENO);
                close(fd[i][1]);
            } else if (i > 0 && i < k - 1) { // both reads and writes to the pipe
                close(fd[i - 1][1]); // Close write end of the previous pipe
                dup2(fd[i - 1][0], STDIN_FILENO); // Redirect input from the previous pipe
                close(fd[i - 1][0]); // Close read end of the previous pipe

                close(fd[i][0]);
                dup2(fd[i][1], STDOUT_FILENO);
                close(fd[i][1]);
            } else if (i == k - 1) { // only reads from the pipe
                close(fd[i - 1][1]); // Close write end of the previous pipe
                dup2(fd[i - 1][0], STDIN_FILENO); // Redirect input from the previous pipe
                close(fd[i - 1][0]); // Close read end of the previous pipe
            }
            execvp(argument_tokens[0], argument_tokens);
        }
        
        for (int j = 0; j < k; j++) {
            close(fd[j][0]);
            close(fd[j][1]);
        }
        waitpid(pid, NULL, 0);
        free_tokens(argument_tokens);
    }

    free(input_string);
    free_tokens(list_of_commands);
    return 0;
}