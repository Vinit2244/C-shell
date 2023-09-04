#include "headers.h"

int write_redirect(char** params){
    char* from_file = params[0];
    char* to_file = params[1];

    int rc = fork();
    if (rc < 0) {
        printf("fork: fork failed\n");
        return 0;
    } else if (rc == 0) {
        close(STDOUT_FILENO);
        open(to_file, O_CREAT | O_WRONLY | O_TRUNC, 0644);

        char* myargs[3];
        myargs[0] = strdup(from_file);
        myargs[1] = strdup(to_file);
        myargs[2] = NULL;
        execvp(myargs[0], myargs);
    } else {
        int wc = wait(NULL);
        return 1;
    }
    return 1;
}