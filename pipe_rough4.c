#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

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
    // NOTE: Don't tokenise at quotes (single or double)
    int no_of_characters = 0;
    int idx = 0;
    int q_flag = 0;
    while (str[idx] != '\0') {
        if (str[idx] == '\'' || str[idx] == '"') {
            if (q_flag == 1) q_flag = 0;
            else q_flag = 1;
        }
        if (str[idx] == c && q_flag == 0) no_of_characters++;
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
    int quotes_flag = 0;
    while (str[str_idx] != '\0') {
        if (str[str_idx] == '\'' || str[str_idx] == '"') {
            if (quotes_flag == 1) quotes_flag = 0;
            else quotes_flag = 1;
        }
        if (str[str_idx] == c && quotes_flag == 0) {
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
    int num_of_commands = 0;
    while (list_of_commands[num_of_commands] != NULL) {
        num_of_commands++;
    }

    int num_pipes = num_of_commands - 1;

    // 0 - read
    // 1 - write
    int pipe_fd[num_pipes][2];
    for (int i = 0; i < num_pipes; i++) {
        if (pipe(pipe_fd[i]) < 0) {
            printf("Error occured while piping\n");
            return 1;
        }
    }

    int pid1 = fork();
    if (pid1 < 0) {
        printf("error while fork\n");
        return 1;
    } else if (pid1 == 0) {
        char** argument_tokens = generate_tokens(list_of_commands[0], ' ');

        for (int j = 1; j < num_pipes; j++) {
            close(pipe_fd[j][0]);
            close(pipe_fd[j][1]);
        }

        close(pipe_fd[0][0]);
        dup2(pipe_fd[0][1], STDOUT_FILENO);
        close(pipe_fd[0][1]);

        execvp(argument_tokens[0], argument_tokens);
        printf("error: execvp\n");
        kill(getpid(), SIGTERM);
    }

    for (int i = 1; i < num_of_commands - 1; i++) {
        // running each sub command
        int pid_i = fork();
        if (pid_i < 0) {
            printf("error while fork\n");
            return 1;
        } else if (pid_i == 0) {
            char** argument_tokens = generate_tokens(list_of_commands[i], ' ');

            for (int j = 0; j < num_pipes; j++) {
                if (j == i - 1 || j == i) continue;
                close(pipe_fd[j][0]);
                close(pipe_fd[j][1]);
            }

            // pipe number to take input = i - 1
            close(pipe_fd[i - 1][1]); // closing the write end of pipe i - 1
            // pipe number to write output = i
            close(pipe_fd[i][0]); // closing the read end of pipe i

            dup2(pipe_fd[i - 1][0], STDIN_FILENO);
            dup2(pipe_fd[i][1], STDOUT_FILENO);

            close(pipe_fd[i - 1][0]);
            close(pipe_fd[i][1]);
        }
    }

    int pidk_1 = fork();
    if (pidk_1 < 0) {
        printf("error while fork\n");
        return 1;
    } else if (pidk_1 == 0) {
        char** argument_tokens = generate_tokens(list_of_commands[num_of_commands - 1], ' ');

        for (int j = 0; j < num_pipes - 1; j++) {
            close(pipe_fd[j][0]);
            close(pipe_fd[j][1]);
        }

        close(pipe_fd[num_pipes - 1][1]);
        dup2(pipe_fd[num_pipes - 1][0], STDIN_FILENO);
        close(pipe_fd[num_pipes - 1][0]);

        execvp(argument_tokens[0], argument_tokens);
        printf("error: execvp\n");
        kill(getpid(), SIGTERM);
    }

    // closing all pipes in parent execept the read of last pipe
    for (int i = 0; i < num_pipes; i++) {
        close(pipe_fd[i][0]);
        close(pipe_fd[i][1]);
    }
    for (int i = 0; i < num_of_commands; i++) {
        wait(NULL);
    }

    free(input_string);
    free_tokens(list_of_commands);
    return 0;
}