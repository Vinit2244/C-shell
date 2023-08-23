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

void free_node(LL_Node node) {
    int pid_to_free = node->pid;
    LL_Node trav = LL->first;
    LL_Node prev = NULL;
    while (trav != NULL) {
        if (trav->pid == pid_to_free) {
            if (prev != NULL) {
                prev->next = trav->next;
                free(trav);
                LL->no_of_nodes--;
                if (LL->no_of_nodes == 0) {
                    LL->first = NULL;
                    LL->last = NULL;
                }
                break;
            } else {
                LL->first = trav->next;
                free(trav);
                LL->no_of_nodes--;
                if (LL->no_of_nodes == 0) {
                    LL->first = NULL;
                    LL->last = NULL;
                }
                break;
            }
        }
        prev = trav;
        trav = trav->next;
    }
}

void read_and_free_LL() {
    LL_Node curr = LL->first;
    while (curr != NULL) {
        if (curr->flag == 1) {
            sprintf(bg_process_buffer, "Process exited normally (%d)\n", curr->pid);
            LL_Node temp = curr->next;
            free_node(curr);
            curr = temp;
        } else if (curr->flag == 0) {
            sprintf(bg_process_buffer, "Process exited abnormally (%d)\n", curr->pid);
            LL_Node temp = curr->next;
            free_node(curr);
            curr = temp;
        }
    }
}

void update_LL(int pid, int status) {
    LL_Node trav = LL->first;
    while (trav != NULL) {
        if (trav->pid == pid) {
            trav->flag = status;
        }
        trav = trav->next;
    }
}