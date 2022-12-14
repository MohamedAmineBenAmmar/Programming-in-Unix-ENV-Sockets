#include "../../global/libs.h"
#include "../../global/constants.h"
#include "../../global/types.h"


FILE *fptr;

void close_communication_file()
{
	fptr = fopen(COMMUNICATION, "a");
	fputs("\n}", fptr);
	fclose(fptr);
	kill(getpid(), SIGKILL);
}

int main(int argc, char **argv)
{
	/* Checking if the user entred a port */
	if (argc != 2)
	{
		printf("Usage: %s <port>\n", argv[0]);
		exit(0);
	}

	/* Declare the needed variables */
	int sockfd, ret;
	struct sockaddr_in serverAddr;

	int newSocket;
	struct sockaddr_in newAddr;

	socklen_t addr_size;

	pid_t childpid;

	Request req;
	Response res;
	Ack ack;

	int port = atoi(argv[1]);

	int nbr = 0;
	FILE *cfptr;

	/* Open the communication file */
	fptr = fopen(COMMUNICATION, "w");
	fprintf(fptr, "%s", "{\n\"server\":{\"PID\":");
	fprintf(fptr, "%d", getpid());
	fprintf(fptr, "%s", ", \"port\":");
	fprintf(fptr, "%s", argv[1]);
	fprintf(fptr, "%s", ", \"address\": \"127.0.0.1\"");
	fprintf(fptr, "%s", "}");
	fclose(fptr);

	/* Initialize the signal handler so that the serve know when to close the communication file */
	signal(SIGINT, close_communication_file);

	/* Initialize the random number generator */
	srand(getpid());

	/* Creation of a new socket */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		printf("[-]Error in the server socket creation.\n");
		exit(1);
	}
	printf("[+]Server Socket is created.\n");

	/* Setting the server socket data */
	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	/* Perform the binding */
	ret = bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
	if (ret < 0)
	{
		printf("[-]Error in binding.\n");
		exit(1);
	}
	printf("[+]Bind to port %d\n", port);

	/* Configuring the server to listen to 10 clients max */
	if (listen(sockfd, 10) == 0)
	{
		printf("[+]Listening....\n");
	}
	else
	{
		printf("[-]Error in max clients configuration.\n");
	}

	while (1)
	{
		/* Creation of the client socket */
		newSocket = accept(sockfd, (struct sockaddr *)&newAddr, &addr_size);
		if (newSocket < 0)
		{
			exit(1);
		}

		/* Variables needed to store the ip address and the port of the new clients */
		int new_client_port;
		char new_client_ip_address[15];

		new_client_port = ntohs(newAddr.sin_port);
		strcpy(new_client_ip_address, inet_ntoa(newAddr.sin_addr));

		printf("[+]Connection accepted from %s:%d\n", new_client_ip_address, new_client_port);

		/* Writing the basic clients data to the communication.json file */
		fptr = fopen(COMMUNICATION, "a");
		nbr++;
		fprintf(fptr, "%s", ",\n\"client_");
		fprintf(fptr, "%d", nbr);
		fprintf(fptr, "%s", "\": {");

		fprintf(fptr, "%s", "\"address\":\"");
		fprintf(fptr, "%s", new_client_ip_address);
		fprintf(fptr, "%s", "\", ");
		fprintf(fptr, "%s", "\"port\": \"");
		fprintf(fptr, "%d", new_client_port);
		fprintf(fptr, "%s", "\"}");
		fclose(fptr);

		char client_name[20];
		int length = snprintf(NULL, 0, "%d", nbr);
		char *str_nbr = malloc(length + 1);
		snprintf(str_nbr, length + 1, "%d", nbr);

		strcpy(client_name, "client_");
		strcat(client_name, str_nbr);

		char client_filepath[50];
		strcpy(client_filepath, DATA);
		strcat(client_filepath, client_name);
		strcat(client_filepath, ".json");

		/* Creating a tmp client file that holds all the client communication with the client*/
		cfptr = fopen(client_filepath, "w");
		fclose(cfptr);

		/* Creating a child process to manage the client request */
		if ((childpid = fork()) == 0)
		{
			close(sockfd);

			while (1)
			{
				recv(newSocket, &req, sizeof(Request), 0);
				printf("[+]Client with PID %d send %d as payload.\n", req.client_pid, req.data);

				/* Opening the specific client file that will hold his communication with the server */
				cfptr = fopen(client_filepath, "a");
				fprintf(cfptr, "%s", "{");
				fprintf(cfptr, "%s", "\"input\":");
				fprintf(cfptr, "%d", req.data);
				fprintf(cfptr, "%s", ", \"output\": [");

				res.size = req.data;
				for (int i = 0; i < res.size; i++)
				{
					int gen_rand_nbr = (rand() % (UPPER - LOWER + 1)) + LOWER;
					fprintf(cfptr, "%d", gen_rand_nbr);
					if (i + 1 < res.size)
					{
						fprintf(cfptr, "%c", ',');
					}
					res.data[i] = gen_rand_nbr;
				}
				fprintf(cfptr, "%s", "], ");
				fprintf(cfptr, "%s", "\"server_child_pid\":");
				fprintf(cfptr, "%d", getpid());
				fprintf(cfptr, "%s", ", ");
				fprintf(cfptr, "%s", "\"server_pid\":");
				fprintf(cfptr, "%d", getppid());
				fprintf(cfptr, "%s", ", ");
				fprintf(cfptr, "%s", "\"pid\":");
				fprintf(cfptr, "%d", req.client_pid);
				fprintf(fptr, "%s", "}");
				fclose(cfptr);

				res.server_pid = getppid();
				res.server_child_pid = getpid();

				send(newSocket, &res, sizeof(res), 0);
				printf("[+]Data sent to the client %s:%d\n", new_client_ip_address, new_client_port);


				/* Waiting the ack from the client */
				recv(newSocket, &ack, sizeof(Ack), 0);
				if (ack.confirmation == 1)
				{
					printf("[+]The client with %s:%d and a PID of %d has received the server response and confirmed that.\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port), ack.client_pid);
					break;
				}
			}

			exit(1);
		}
	}

	close(newSocket);

	return 0;
}
