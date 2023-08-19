#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <pwd.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    char* input = (char*) calloc(1000, sizeof(char));
    printf("Enter the command to be stored: ");
    scanf("%s", input);

    FILE *fptr;
    fptr = fopen("past_commands.txt", "r");

    if (fptr == NULL) {
        fptr = fopen("past_commands.txt", "w");
        fprintf(fptr, "%s\n", input);
        fclose(fptr);
    } else {
        char** past_commands = (char**) calloc(15, sizeof(char*));
        int write = 1;
        for (int i = 0; i < 15; i++) {
            past_commands[i] = (char*) calloc(1000, sizeof(char));
            fscanf(fptr, "%s\n", past_commands[i]);

            if (i == 0) {
                if (strcmp(input, past_commands[0]) == 0) {
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
            fprintf(fptr2, "%s\n", input);
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
    return 0;
}