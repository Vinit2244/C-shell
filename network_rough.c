#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>

// standard http port
#define SERVER_PORT 80

#define MAX_LEN 100000

#define SA struct sockaddr

void remove_leading_and_trailing_spaces(char* str) {
    // removing leading spaces 
    int front_index = 0;
    while (str[front_index] == ' ' || str[front_index] == '\t') {
        front_index++;
    }
    int rear_index = strlen(str) - 1;
    while (str[rear_index] == ' ' || str[rear_index] == '\t') {
        rear_index--;
    }
    if (rear_index < front_index) {
        str[0] = '\0';
        return;
    }
    int len = rear_index - front_index + 1;
    char* final = (char*) malloc(sizeof(char) * (len + 1));
    for (int i = 0; i < len; i++) {
        final[i] = str[front_index++];
    }
    final[len] = '\0';
    for (int i = 0; i < len + 1; i++) {
        str[i] = final[i];
    }
    free(final);
}

char** generate_tokens(char* str, char c) {
    int no_of_characters = 0;
    int idx = 0;
    while (str[idx] != '\0') {
        if (str[idx] == c) no_of_characters++;
        idx++;
    }
    int no_of_partitions = no_of_characters + 1;
    char** tokens_array = (char**) calloc(no_of_partitions + 1, sizeof(char*));
    for (int i = 0; i < no_of_partitions; i++) {
        tokens_array[i] = (char*) calloc(MAX_LEN, sizeof(char));
    }
    tokens_array[no_of_partitions] = NULL;
    int str_idx = 0;
    int tokens_array_idx = 0;
    int token_idx = 0;
    while (str[str_idx] != '\0') {
        if (str[str_idx] == c) {
            tokens_array[tokens_array_idx][token_idx] = '\0';
            token_idx = 0;
            tokens_array_idx++;
        } else {
            tokens_array[tokens_array_idx][token_idx] = str[str_idx];
            token_idx++;
        }
        str_idx++;
    }

    tokens_array[tokens_array_idx][token_idx] = '\0';

    // Removing all the leading and trailing spaces from the commands if any present
    for (int i = 0; i <= tokens_array_idx; i++) {
        remove_leading_and_trailing_spaces(tokens_array[i]);
    }
    return tokens_array;
}

void free_tokens(char** tokens) {
    int idx = 0;
    while (tokens[idx] != NULL) {
        free(tokens[idx]);
        idx++;
    }
    free(tokens);
}

int main() {
    // socket_fd = socket file descriptor (just acts like a file)
    int socket_fd, n;
    int num_of_bytes_to_be_sent;
    struct sockaddr_in server_addr;
    char sendline[MAX_LEN];
    char recvline[MAX_LEN];

    char* input_string = (char*) calloc(5000, sizeof(char));
    input_string[0] = '\0';

    printf("Enter hostname: ");

    fgets(input_string, 4096, stdin);
    input_string[strlen(input_string)] = '\0';

    // removing endline character
    for (int i = 0; i < strlen(input_string); i++) {
        if (input_string[i] != '\n') continue;
        else {
            input_string[i] = '\0';
        }
    }

    // DNS resolution
    // char hostname[] = "man.he.net";
    char* hostname = input_string;
    struct addrinfo specs;

    memset(&specs, 0, sizeof(specs)); // initializing to 0

    specs.ai_family = AF_UNSPEC;     // Address family unspecified (return both IPv4 and IPv6 addresses)
    specs.ai_socktype = SOCK_STREAM; // TCP

    struct addrinfo *LL_of_addresses;
    // resolving DNS
    int status = getaddrinfo(hostname, NULL, &specs, &LL_of_addresses); // returns a linked list of struct addrinfo stored at position LL_of_addresses
    if (status != 0) {
        printf("getaddrinfo: %s\n", gai_strerror(status)); // gai_strerror() converts the error code returned by the getaddrinfo() function in string form
        return 1;
    }

    // obtaining the ip address of the input hostname
    void *addr; // stores binary IP address
    char ip[MAX_LEN];

    // Iterating through all the addresses returned
    struct addrinfo *trav = LL_of_addresses;
    while (trav != NULL) {
        // trav->ai_addr is a generic structure that can hold information about both ipv4 and ipv6 address so first we need to check which address are we talking about and then accordingly resolve the sockaddr_in struct
        if (trav->ai_family == AF_INET) { // IPv4
            struct sockaddr_in *ipv4 = (struct sockaddr_in *) trav->ai_addr;
            addr = &(ipv4->sin_addr);
        } else { // IPv6 (not using IPv6 only using IPv4)
            trav = trav->ai_next;
            continue;
        }
        // Converting biary IP address to string
        inet_ntop(trav->ai_family, addr, ip, MAX_LEN);
        trav = trav->ai_next;
    }
    freeaddrinfo(LL_of_addresses);

    // Address Family - Internet, STREAM socket, 0 == USE TCP
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("socket: Error while creating socket\n");
        return 1;
    }

    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET; 
    server_addr.sin_port = htons(SERVER_PORT); // host to network, short (converts the host my byte order (bigendian or little-endian) to network byte order)

    // converting string ip to binary again (translation)
    if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
        printf("Error while converting string IP to binary\n");
        return 1;
    }

    // trying to connect to the server
    if (connect(socket_fd, (SA *) &server_addr, sizeof(server_addr)) < 0) {
        printf("connection failed: Error while connecting to the server\n");
        return 1;
    }

    // writing onto the array the GET request we need to send to the server
    char cmd[] = "sleep";
    sprintf(sendline, "GET /?topic=%s&section=all HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n",cmd, hostname); // \r\n\r\n marks the end of request (Here we are using HTTP version 1.1 and we are requesting the '/' root page)
    num_of_bytes_to_be_sent = strlen(sendline); // number of bytes that would be sent if the whole query/request is sent succesfully
    
    // write the request onto the socket file descriptor
    if (write(socket_fd, sendline, num_of_bytes_to_be_sent) != num_of_bytes_to_be_sent) {
        // if the number of bytes sent is not equal to the number of bytes we need to send then we know that there is an error in the request
        printf("GET: Request not sent properly\n");
        return 1;
    }

    // resetting the read buffer to 0 to ensure erasure of previous data
    memset(recvline, 0, MAX_LEN);
    int print_flag = 0;

    // keep reading from the socket file descriptor as long as data is available in there or the data is constantly getting written onto the fd by the server so keep reading it untill all data is read
    while ((n = read(socket_fd, recvline, MAX_LEN - 1)) > 0) {
        
        char** tkns = generate_tokens(recvline, '\n');
        int y = 0;
        while (tkns[y] != NULL) {
            if (print_flag == 1 && strcmp(tkns[y], "AUTHOR") != 0) {
                printf("%s\n", tkns[y]);
            } else if (print_flag == 0 && strcmp(tkns[y], "NAME") == 0) {
                printf("%s\n", tkns[y]);
                print_flag = 1;
            } else if (print_flag == 1 && strcmp(tkns[y], "AUTHOR") == 0) {
                print_flag = 0;
                break;
            }
            y++;
        }
        free_tokens(tkns);
    }
    if (n < 0) {
        printf("read error: could not read the received data\n");
    }

    // close(socket_fd);
    return 0;
}

