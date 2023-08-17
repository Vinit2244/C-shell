#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <pwd.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    char home_directory[10000]; // stores the absolute path to the home directory
    char cwd[10000];            // stores the absolute path to the current working directory
    
    char* buff1 = home_directory;
    char* buff2 = cwd;
    do {
        char* buff1 = getcwd(home_directory, 9999);
        char* buff2 = getcwd(cwd, 9999);          // initially current working directory will be same as the home_directory
    } while (buff1 == NULL || buff2 == NULL);

    char* next_dir_to_go_to = "test";

    DIR *dr;
    struct dirent *en;
    dr = opendir(cwd);
    int flag = 0;
    if (dr) {
        while ((en = readdir(dr)) != NULL) {
            printf("%s\n", en->d_name);
            if (strcmp(en->d_name, next_dir_to_go_to) == 0) {
                flag = 1;
            }
        }
        closedir(dr);
    }
    int start = strlen(cwd);
    cwd[start] = '/';
    for (int i = start + 1; i < start + 1 + strlen(next_dir_to_go_to); i++) {
        cwd[i] = next_dir_to_go_to[i - start - 1];
    }
    printf("%s\n", cwd);

    DIR *dr1;
    struct dirent *en1;
    dr1 = opendir(cwd);
    if (dr1) {
        while ((en1 = readdir(dr1)) != NULL) {
            printf("%s\n", en1->d_name);
        }
        closedir(dr1);
    }

    return 0;
}