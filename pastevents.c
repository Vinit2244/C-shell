#include "headers.h"

void store_command(char* command, char* home_dir) {
    char past_commands_path[MAX_LEN];
    strcpy(past_commands_path, home_dir);
    strcat(past_commands_path, "/");
    strcat(past_commands_path, "past_commands.txt");
    FILE *fptr;
    fptr = fopen(past_commands_path, "r");

    if (fptr == NULL) {
        fptr = fopen(past_commands_path, "w");
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
            fptr2 = fopen(past_commands_path, "w");
            if (fptr2 == NULL) {

                printf("\033[1;31mpastevents: error opening file for writing\033[1;0m\n");
            } else {
                fprintf(fptr2, "%s\n", command);
                for (int i = 0; i < 14; i++) {
                    if (past_commands[i][0] != '\0') {
                        fprintf(fptr2, "%s\n", past_commands[i]);
                    } else {
                        break;
                    }
                }
                fclose(fptr2);
            }
            for (int i = 0; i < 15; i++) {
                free(past_commands[i]);
            }
            free(past_commands);
        }
    }
}

void pastevents(char* home_dir) {
    char past_commands_path[MAX_LEN] = {0};
    strcpy(past_commands_path, home_dir);
    strcat(past_commands_path, "/past_commands.txt");

    FILE *fptr;
    fptr = fopen(past_commands_path, "r");

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
            if (past_commands[i][0] != '\0'){
                char buff[MAX_LEN] = {0};
                sprintf(buff, "%s\n", past_commands[i]);
                bprintf(global_buffer, buff);
            }
        }

        for (int i = 0; i < 15; i++) {
            free(past_commands[i]);
        }
        free(past_commands);
    }
}

int purge(int ap, int w, char* home_dir) {
    FILE *fptr;
    char past_commands_path[MAX_LEN] = {0};
    strcpy(past_commands_path, home_dir);
    strcat(past_commands_path, "/past_commands.txt");

    fptr = fopen(past_commands_path, "w");
    if (fptr == NULL) {
        printf("\033[1;31mpastevents: error opening file for writing\033[1;0m\n");
        return 0;
    }
    fclose(fptr);
    return 1;
}

int execute(int num, char* home_dir, char* cwd, char* prev_dir, int store, char* last_command, int* t, int ap, int w) {
    FILE *fptr;
    char past_commands_path[MAX_LEN] = {0};
    strcpy(past_commands_path, home_dir);
    strcat(past_commands_path, "/past_commands.txt");

    fptr = fopen(past_commands_path, "r");

    if (fptr == NULL) {
        printf("\033[1;31mFile Not Found\033[1;0m\n");
        return 0;
    } else {
        char** past_commands = (char**) calloc(15, sizeof(char*));

        for (int i = 0; i < 15; i++) {
            past_commands[i] = (char*) calloc(MAX_LEN, sizeof(char));
            fscanf(fptr, " %[^\n]", past_commands[i]);
        }
        fclose(fptr);

        if (past_commands[num - 1][0] == '\0') {
            printf("\033[1;31mInvalid Argument\033[1;0m\n");
            return 0;
        } else {
            input(past_commands[num - 1], home_dir, cwd, prev_dir, store, last_command, t, 0, 0, 0, NULL, NULL);
        }

        for (int i = 0; i < 15; i++) {
            free(past_commands[i]);
        }
        free(past_commands);
    }
    return 1;
}

