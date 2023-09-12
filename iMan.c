#include "headers.h"

// standard http port
#define SERVER_PORT 80

#define SA struct sockaddr

int get_webpage(char* command_name) {
    // socket_fd = socket file descriptor (just acts like a file)
    int socket_fd;
    int num_of_bytes_to_be_sent;
    struct sockaddr_in server_addr;
    char sendline[MAX_LEN]; // string to send
    char recvline[MAX_LEN]; // buffer to read the received data

    // DNS resolution
    char hostname[] = "man.he.net";
    struct addrinfo specs;

    memset(&specs, 0, sizeof(specs)); // initializing to 0

    specs.ai_family = AF_UNSPEC;     // Address family unspecified (return both IPv4 and IPv6 addresses)
    specs.ai_socktype = SOCK_STREAM; // TCP

    struct addrinfo *LL_of_addresses;
    // resolving DNS
    int status = getaddrinfo(hostname, NULL, &specs, &LL_of_addresses); // returns a linked list of struct addrinfo stored at position LL_of_addresses
    if (status != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status)); // gai_strerror() converts the error code returned by the getaddrinfo() function in string form
        return 0;
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
        fprintf(stderr, "\033[1;31msocket: Error while creating socket\033[1;0m\n");
        return 0;
    }

    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET; 
    server_addr.sin_port = htons(SERVER_PORT); // host to network, short (converts the host my byte order (bigendian or little-endian) to network byte order)

    // converting string ip to binary again (translation)
    if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
        fprintf(stderr, "\033[1;31mError while converting string IP to binary\033[1;0m\n");
        return 0;
    }

    // trying to connect to the server
    if (connect(socket_fd, (SA *) &server_addr, sizeof(server_addr)) < 0) {
        fprintf(stderr, "\033[1;31mconnection failed: Error while connecting to the server\033[1;0m\n");
        return 0;
    }

    // writing onto the array the GET request we need to send to the server
    sprintf(sendline, "GET /?topic=%s&section=all HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", command_name, hostname); // \r\n\r\n marks the end of request (Here we are using HTTP version 1.1 and we are requesting the '/' root page)
    num_of_bytes_to_be_sent = strlen(sendline); // number of bytes that would be sent if the whole query/request is sent succesfully
    
    // write the request onto the socket file descriptor
    if (write(socket_fd, sendline, num_of_bytes_to_be_sent) != num_of_bytes_to_be_sent) {
        // if the number of bytes sent is not equal to the number of bytes we need to send then we know that there is an error in the request
        fprintf(stderr, "\033[1;31mGET: Request not sent properly\033[1;0m\n");
        return 0;
    }

    // resetting the read buffer to 0 to ensure erasure of previous data
    memset(recvline, 0, MAX_LEN);
    int print_flag = 0;
    int overall_flag = 0;

    int n;
    // keep reading from the socket file descriptor as long as data is available in there or the data is constantly getting written onto the fd by the server so keep reading it untill all data is read
    while ((n = read(socket_fd, recvline, MAX_LEN - 1)) > 0) {
        char** tkns = generate_tokens(recvline, '\n');
        int y = 0;
        while (tkns[y] != NULL) {
            if (print_flag == 1 && strcmp(tkns[y], "AUTHOR") != 0) {
                overall_flag = 1;
                char buff[MAX_LEN] = {0};
                sprintf(buff, "%s\n", tkns[y]);
                bprintf(global_buffer, buff);
            } else if (print_flag == 0 && strcmp(tkns[y], "NAME") == 0) {
                char buff[MAX_LEN] = {0};
                sprintf(buff, "%s\n", tkns[y]);
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
        fprintf(stderr, "\033[1;31mread error: could not read the received data\033[1;0m\n");
        close(socket_fd);
        return 0;
    }

    if (overall_flag == 0) { // if nothing is printed then we know that page not found is returned
        fprintf(stderr, "\033[1;31mERROR\033[1;0m\n");
        fprintf(stderr, "\t\033[1;31mNo such Command\033[1;0m\n");
        return 0;
    }

    // closing the socket
    close(socket_fd);
    return 1;
}

