#ifndef __PING_H
#define __PING_H

int ping(char** argument_tokens, int no_of_arguments, int ap, int w);
int ping_sig(int pid, int sig, int ap, int w);

#endif