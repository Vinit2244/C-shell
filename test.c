#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <pwd.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    char* str = (char*) calloc(100, sizeof(char));
    strcpy(str, "Hello");
    printf("%s\n", str);
    sprintf(str, "%d", 1234);
    printf("%s\n", str);
    return 0;
}