#include "headers.h"

linked_list_head create_linked_list_head() {
    linked_list_head linked_list = (linked_list_head) malloc(sizeof(linked_list_head_struct));
    linked_list->number_of_nodes = 0;
    linked_list->first = NULL;
    linked_list->last = NULL;
    return linked_list;
}

linked_list_node create_linked_list_node(char* path) {
    linked_list_node N = (linked_list_node) malloc(sizeof(linked_list_node_struct));
    N->path = (char*) calloc(MAX_LEN, sizeof(char));
    strcpy(N->path, path);
    return N;
}

void insert_in_linked_list(linked_list_head linked_list, char* path) {
    linked_list_node N = create_linked_list_node(path);
    if (linked_list->number_of_nodes == 0) {
        linked_list->first = N;
        linked_list->last = N;
        linked_list->number_of_nodes++;
    } else if (linked_list->number_of_nodes == 1) {
        linked_list->last = N;
        linked_list->first->next = N;
        linked_list->number_of_nodes++;
    } else {
        linked_list->last->next = N;
        linked_list->last = N;
        linked_list->number_of_nodes++;
    }
}

void free_linked_list(linked_list_head linked_list) {
    linked_list_node trav = linked_list->first;
    while (trav != NULL) {
        free(trav->path);
        linked_list_node temp = trav->next;
        free(trav);
        trav = temp;
    }
    free(linked_list);
}

void traverse_and_print(linked_list_head linked_list, int file, int dir, char* path_to_base_dir, int ap, int w) {
    linked_list_node trav = linked_list->first;
    while (trav != NULL) {
        struct stat dir_stat;
        stat(trav->path, &dir_stat);

        char* p = relative_path(trav->path, path_to_base_dir);

        if (S_ISDIR(dir_stat.st_mode) == 0 && file) {
            if (ap == 1 || w == 1) {
                char buff[MAX_LEN] = {0};
                sprintf(buff, "%s\n", p);
                bprintf(global_buffer, buff);
            }
            else {
                char buff[MAX_LEN] = {0};
                sprintf(buff, "\033[1;32m%s\033[1;0m\n", p);
                bprintf(global_buffer, buff);
            }
        } else if (S_ISDIR(dir_stat.st_mode) != 0 && dir) {
            if (ap == 1 || w == 1) {
                char buff[MAX_LEN] = {0};
                sprintf(buff, "%s\n", p);
                bprintf(global_buffer, buff);
            }
            else {
                char buff[MAX_LEN] = {0};
                sprintf(buff, "\033[1;34m%s\033[1;0m\n", p);
                bprintf(global_buffer, buff);
            }
        }
        trav = trav->next;
        free(p);
    }
}

