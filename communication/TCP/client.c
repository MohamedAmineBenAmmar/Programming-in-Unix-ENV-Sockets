#include "../../global/libs.h"
#include "../../global/constants.h"
#include "../../global/types.h"

int main(int argc, char **argv)
{
	/* Checking if the user entred a port */
	if (argc != 2)
	{
		printf("Usage: %s <port>\n", argv[0]);
		exit(0);
	}

	/* Declare the needed variables */
	int clientSocket, ret;
	struct sockaddr_in serverAddr;

	Request req;
	Response res;
	Ack ack;
	int data;

	int port = atoi(argv[1]);

	/* Init the random number generator */
	srand(getpid());

	/* Creating the client socket */
	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket < 0)
	{
		printf("[-]Error in the client socket creation.\n");
		exit(1);
	}
	printf("[+]Client Socket is created.\n");

	/* Configuring server socket data */
	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	/* Establishing the connection with the server */
	ret = connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
	if (ret < 0)
	{
		printf("[-]Error in connection.\n");
		exit(1);
	}
	printf("[+]Connected to Server.\n");

	/* Generating a random number */
	data = (rand() % (UPPER - LOWER + 1)) + LOWER;
	printf("[+]Client random generated number: %d \n", data);

	/* Preparing the request */
	req.data = data;
	req.client_pid = getpid();

	/* Send a request to the server */
	send(clientSocket, &req, sizeof(req), 0);

	/* Receiving response from the server */
	if (recv(clientSocket, &res, sizeof(Response), 0) < 0)
	{
		printf("[-]Error in receiving data.\n");
	}
	else
	{
		printf("[+]Server responded with:\t");
		for (int i = 0; i < res.size; i++)
		{
			printf("%d ", res.data[i]);
		}
		printf("\n");
	}

	/* Preparing the ack */
	ack.client_pid = getpid();
	ack.confirmation = 1;

	/* Sending ack to the server */
	printf("[+]Sending ack to the server\n");
	send(clientSocket, &ack, sizeof(ack), 0);

	close(clientSocket);
	printf("[-]Disconnected from server.\n");

	return 0;
}
