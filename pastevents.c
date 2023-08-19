#include "headers.h"

void store_command(char* command) {

    FILE *fptr;
    fptr = fopen("past_commands.txt", "r");

    if (fptr == NULL) {
        fptr = fopen("past_commands.txt", "w");
        fprintf(fptr, "%s\n", command);
        fclose(fptr);
    } else {
        char** past_commands = (char**) calloc(15, sizeof(char*));
        int write = 1;
        for (int i = 0; i < 15; i++) {
            past_commands[i] = (char*) calloc(MAX_LEN, sizeof(char));
            fscanf(fptr, " %[^\n]", past_commands[i]);

            if (i == 0) {
                if (strcmp(command, past_commands[0]) == 0) {
                    write = 0;
                    break;
                }
            }
        }

        if (write == 0) {
            fclose(fptr);
            for (int i = 0; i < 15; i++) {
                free(past_commands[i]);
            }
            free(past_commands);
        } else {
            fclose(fptr);

            FILE *fptr2;
            fptr2 = fopen("past_commands.txt", "w");
            fprintf(fptr2, "%s\n", command);
            for (int i = 0; i < 14; i++) {
                if (past_commands[i][0] != '\0') {
                    fprintf(fptr2, "%s\n", past_commands[i]);
                } else {
                    break;
                }
            }
            fclose(fptr2);

            for (int i = 0; i < 15; i++) {
                free(past_commands[i]);
            }
            free(past_commands);
        }
    }
}

void pastevents() {
    FILE *fptr;
    fptr = fopen("past_commands.txt", "r");

    if (fptr == NULL) {
        return;
    } else {
        char** past_commands = (char**) calloc(15, sizeof(char*));
        int write = 1;
        for (int i = 0; i < 15; i++) {
            past_commands[i] = (char*) calloc(MAX_LEN, sizeof(char));
            fscanf(fptr, " %[^\n]", past_commands[i]);
        }

        for (int i = 14; i >= 0; i--) {
            if (past_commands[i][0] != '\0') printf("%s\n", past_commands[i]);
        }

        for (int i = 0; i < 15; i++) {
            free(past_commands[i]);
        }
        free(past_commands);
    }
}

void purge() {
    FILE *fptr;
    fptr = fopen("past_commands.txt", "w");
    fclose(fptr);
}

int execute(int num, char* home_dir, char* cwd, char* prev_dir, int store) {
    FILE *fptr;
    fptr = fopen("past_commands.txt", "r");

    if (fptr == NULL) {
        return 0;
    } else {
        char** past_commands = (char**) calloc(15, sizeof(char*));

        for (int i = 0; i < 15; i++) {
            past_commands[i] = (char*) calloc(MAX_LEN, sizeof(char));
            fscanf(fptr, " %[^\n]", past_commands[i]);
        }
        fclose(fptr);

        int count = 1;
        for (int i = 14; i >= 0; i--) {
            if (past_commands[i][0] == '\0') {
                continue;
            } else {
                if (count == num) {
                    input(past_commands[i], home_dir, cwd, prev_dir, store);
                    break;
                } else {
                    count++;
                }
            }
        }

        for (int i = 0; i < 15; i++) {
            free(past_commands[i]);
        }
        free(past_commands);
    }
    return 1;
}

