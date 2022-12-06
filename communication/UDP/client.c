#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

struct Person
{
    /* data */
    char name[50];
};

int main (int argc, char **argv){
    // Test
    struct Person p1;
    strcpy(p1.name, "Hamma is the best");
    // end test
    
    // Arguments validation
    if(argc != 2){
        printf("Usage error: You must specify a port number\n");
        exit(0);
    }

    // Setting the ip address manually
    char *ip = "127.0.0.1";

    // Getting the port number from the command line as argument
    int port = atoi(argv[1]);

    // Init the needed variables
    int socketfd;
    struct sockaddr_in addr;
    char buffer[1024];
    socklen_t addr_size;

    // Initializing the socket (UDP)
    socketfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(socketfd < 0){
        perror("[-]socket error\n");
        exit(1);
    }

    // Setting some values in the client socket
    memset(&addr, '\0', sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);

    // Sending data from the client to the server
    bzero(buffer, 1024);
    
    strcpy(buffer, "yo from the shity client");
    // sendto(socketfd, buffer, 1024, 0, (struct sockaddr*)&addr, sizeof(addr));
    // printf("[+]Data sent to the server %s\n", buffer);
    sendto(socketfd, &p1, sizeof(struct Person), 0, (struct sockaddr*)&addr, sizeof(addr));
    printf("[+]Data sent to the server %s\n", p1.name);


    // Receving response from the server
    bzero(buffer, 1024);
    addr_size = sizeof(addr);
    recvfrom(socketfd, buffer, 1024, 0, (struct sockaddr*)&addr, &addr_size);
    printf("[+]Data received from the server %s\n", buffer);

    close(socketfd);
    return 0;
}