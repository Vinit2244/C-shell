#include "headers.h"

LL_Head create_LL() {
    LL_Head LL = (LL_Head) malloc(sizeof(LL_Head_struct));
    LL->no_of_nodes = 0;
    LL->first = NULL;
    LL->last = NULL;
    return LL;
}

LL_Node create_node(int pid, int flag) {
    LL_Node N = (LL_Node) malloc(sizeof(LL_Node_struct));
    N->pid = pid;
    N->flag = flag;
    N->next = NULL;
    return N;
}

void insert_in_LL(int pid, int flag) {
    LL_Node N = create_node(pid, flag);
    if (LL->no_of_nodes == 0) {
        LL->first = N;
        LL->last = N;
        LL->no_of_nodes++;
    } else if (LL->no_of_nodes == 1) {
        LL->last = N;
        LL->first->next = N;
        LL->no_of_nodes++;
    } else {
        LL->last->next = N;
        LL->last = N;
        LL->no_of_nodes++;
    }
}

void read_and_free_LL() {
    LL_Node curr = LL->first;
    while (curr != NULL) {
        if (curr->flag == 1) {
            printf("Process exited normally (%d)\n", curr->pid);
        } else {
            printf("Process exited abnormally (%d)\n", curr->pid);
        }
        LL->first = curr->next;
        free(curr);
        curr = LL->first;
    }
    LL->last = NULL;
    LL->no_of_nodes = 0;
}