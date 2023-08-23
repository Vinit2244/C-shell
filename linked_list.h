#ifndef __LINKED_LIST_H
#define __LINKED_LIST_H

typedef struct LL_Head_struct* LL_Head;
typedef struct LL_Node_struct* LL_Node;

typedef struct LL_Head_struct {
    int no_of_nodes;
    struct LL_Node_struct* first;
    struct LL_Node_struct* last;
} LL_Head_struct;

typedef struct LL_Node_struct {
    int pid;
    int flag;
    struct LL_Node_struct* next;
} LL_Node_struct;

extern struct LL_Head_struct* LL;
extern char* bg_process_buffer;

LL_Head create_LL();
LL_Node create_node(int pid, int flag);
void insert_in_LL(int pid, int flag);
void free_node(LL_Node node);
void read_and_free_LL();
void update_LL(int pid, int status) ;

#endif