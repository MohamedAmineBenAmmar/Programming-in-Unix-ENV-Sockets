#include "../../global/libs.h"
#include "../../global/constants.h"
#include "../../global/types.h"

#define PORT 4444

int main()
{
	int clientSocket, ret;
	struct sockaddr_in serverAddr;
	char buffer[1024];

	Request req;
	Response res;
	Ack ack;
	int data;

	/* Init the random number generator */
	srand(getpid());

	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket < 0)
	{
		printf("[-]Error in connection.\n");
		exit(1);
	}
	printf("[+]Client Socket is created.\n");

	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	ret = connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
	if (ret < 0)
	{
		printf("[-]Error in connection.\n");
		exit(1);
	}
	printf("[+]Connected to Server.\n");

	data = (rand() % (UPPER - LOWER + 1)) + LOWER;
	printf("Client random generated number: %d \n", data);

	req.data = data;
	req.client_pid = getpid();

	send(clientSocket, &req, sizeof(req), 0);
	if (recv(clientSocket, &res, sizeof(Response), 0) < 0)
	{
		printf("[-]Error in receiving data.\n");
	}
	else
	{
		printf("Server responded with:\t");
		for (int i = 0; i < res.size; i++)
		{
			printf("%d ", res.data[i]);
		}
		printf("\n");
	}

	ack.client_pid = getpid();
	ack.confirmation = 1;
	send(clientSocket, &ack, sizeof(ack), 0);

	close(clientSocket);
	printf("[-]Disconnected from server.\n");

	return 0;
}
