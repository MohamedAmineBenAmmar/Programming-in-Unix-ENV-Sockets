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

  /* Initializing the ip address and the port */
  char *ip = "127.0.0.1";
  int port = atoi(argv[1]);

  /* Declare the needed variables */
  int sockfd;    // Server socket for listening to the main clients requests
  int acksockfd; // Secondary socket for listening to the clients acks

  struct sockaddr_in server_addr, client_addr, server_ack_addr, client_ack_addr;
  socklen_t addr_size, client_ack_addr_size;
  int n;
  pid_t childpid;

  Request req;
  Response res;
  Ack ack;

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

  /* Create the socket that is going to listen to clients requests */
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0)
  {
    perror("[-]Error in the server socket creation. \n");
    exit(1);
  }
  printf("[+]Server Socket is created.\n");

  /* Create the socket that is going to listen for the clients ack */
  acksockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (acksockfd < 0)
  {
    perror("[-]Ack server socket error.\n");
    exit(1);
  }
  printf("[+]Ack server Socket is created.\n");

  /* Setting the server socket data */
  memset(&server_addr, '\0', sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  server_addr.sin_addr.s_addr = inet_addr(ip);

  /* Setting the server ack socket data */
  memset(&server_ack_addr, '\0', sizeof(server_ack_addr));
  server_ack_addr.sin_family = AF_INET;
  server_ack_addr.sin_port = htons(3000);
  server_ack_addr.sin_addr.s_addr = inet_addr(ip);

  /* Binding the sockets with the appropriate config */
  n = bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (n < 0)
  {
    perror("[-]Main socket bind error.\n");
    exit(1);
  }
  printf("[+]Bind main entry to port %d\n", port);

  n = bind(acksockfd, (struct sockaddr *)&server_ack_addr, sizeof(server_ack_addr));
  if (n < 0)
  {
    perror("[-]Ack socket bind error.\n");
    exit(1);
  }
  printf("[+]Bind ack to port %d\n", 3000);

  addr_size = sizeof(client_addr);

  while (1)
  {
    recvfrom(sockfd, &req, sizeof(Request), 0, (struct sockaddr *)&client_addr, &addr_size);
    printf("[+]Client IP adress: %s and port: %d with PID %d send %d as payload.\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), req.client_pid, req.data);

    /* Variables needed to store the ip address and the port of the new clients */
    int new_client_port;
    char new_client_ip_address[15];
    new_client_port = ntohs(client_addr.sin_port);
    strcpy(new_client_ip_address, inet_ntoa(client_addr.sin_addr));

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

    if ((childpid = fork()) == 0)
    {
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
      fprintf(cfptr, "%s", "}");
      fclose(cfptr);

      res.server_pid = getppid();
      res.server_child_pid = getpid();

      sendto(sockfd, &res, sizeof(res), 0, (struct sockaddr *)&client_addr, sizeof(client_addr));
      printf("[+]Data sent to the client %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

      /* Waiting the ack from the client */
      recvfrom(acksockfd, &ack, sizeof(Ack), 0, (struct sockaddr *)&client_ack_addr, &client_ack_addr_size);
      if (ack.confirmation == 1)
      {
        printf("[+]Received ack from %s:%d\n", inet_ntoa(client_ack_addr.sin_addr), ntohs(client_ack_addr.sin_port));
      }
      else
      {
        printf("[-]Error occured while  receiving the ack\n");
      }
      exit(1);
    }
  }

  close(sockfd);
  close(acksockfd);

  return 0;
}
