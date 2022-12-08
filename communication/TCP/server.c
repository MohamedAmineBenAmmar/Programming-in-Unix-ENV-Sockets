#include "../../global/libs.h"
#include "../../global/constants.h"
#include "../../global/types.h"

#define PORT 4444

int main()
{

	/* Declare the needed variables */
	int sockfd, ret;
	struct sockaddr_in serverAddr;

	int newSocket;
	struct sockaddr_in newAddr;

	socklen_t addr_size;

	char buffer[1024];
	pid_t childpid;

	Request req;
	Response res;
	Ack ack;

	/* Initialize the random number generator */
	srand(getpid());

	/* Creation of a new socket */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		printf("[-]Error in connection.\n");
		exit(1);
	}
	printf("[+]Server Socket is created.\n");

	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	ret = bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
	if (ret < 0)
	{
		printf("[-]Error in binding.\n");
		exit(1);
	}
	printf("[+]Bind to port %d\n", 4444);

	if (listen(sockfd, 10) == 0)
	{
		printf("[+]Listening....\n");
	}
	else
	{
		printf("[-]Error in binding.\n");
	}

	while (1)
	{
		newSocket = accept(sockfd, (struct sockaddr *)&newAddr, &addr_size);
		if (newSocket < 0)
		{
			exit(1);
		}
		printf("Connection accepted from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));

		if ((childpid = fork()) == 0)
		{
			close(sockfd);

			while (1)
			{
				recv(newSocket, &req, sizeof(Request), 0);
				printf("[+]Client with PID %d send %d as payload.\n", req.client_pid, req.data);
				
				res.size = req.data;
				for (int i = 0; i < res.size; i++)
				{
					res.data[i] = (rand() % (UPPER - LOWER + 1)) + LOWER;
				}

				res.server_pid = getppid();
				res.server_child_pid = getpid();

				send(newSocket, &res, sizeof(res), 0);

				recv(newSocket, &ack, sizeof(Ack), 0);
				if(ack.confirmation == 1){
					printf("The client with %s:%d and a PID of %d has received the server response and confirmed that.\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port), ack.client_pid);
					break;
				}
			}
		}
	}

	close(newSocket);

	return 0;
}
