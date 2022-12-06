#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>


struct Person
{
    /* data */
    char name[50];
};

int main(int argc, char **argv)
{
    // Test
    struct Person p1;
    // strcpy(p1.name, "Hamma is the best");
    // end test

    // Arguments validation
    if (argc != 2)
    {
        printf("Usage error: You must specify a port number\n");
        exit(0);
    }

    // Setting the ip address manually
    char *ip = "127.0.0.1";

    // Getting the port number from the command line as argument
    int port = atoi(argv[1]);

    // Init the needed variables
    int socketfd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[1024];
    socklen_t addr_size;
    int n;

    // Initializing the socket (UDP)
    socketfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketfd < 0)
    {
        perror("[-]socket error\n");
        exit(1);
    }

    // Setting some values in the server socket
    memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    // Setting the bind
    n = bind(socketfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (n < 0)
    {
        perror("[-]bind error\n");
        exit(1);
    }

    while (1)
    {
        // Listening to upcomong requests
        printf("Server is listening at port: %d\n", port);

        // Receiving data from the client
        bzero(buffer, 1024);
        addr_size = sizeof(client_addr);
        // recvfrom(socketfd, buffer, 1024, 0, (struct sockaddr *)&client_addr, &addr_size);
        // printf("[+]Data received from the client %s\n", buffer);
        recvfrom(socketfd, &p1, sizeof(struct Person), 0, (struct sockaddr *)&client_addr, &addr_size);
        printf("[+]Data received from the client %s\n", p1.name);

        // Send the data back to the client
        bzero(buffer, 1024);
        strcpy(buffer, "UDP server bankai");
        sendto(socketfd, buffer, 1024, 0, (struct sockaddr *)&client_addr, sizeof(client_addr));
        printf("[+]Data sent to the client %s\n", buffer);
    }

    close(socketfd);
    return 0;
}