#include "../../global/libs.h"
#include "../../global/constants.h"
#include "../../global/types.h"

int main(int argc, char **argv)
{

  if (argc != 2)
  {
    printf("Usage: %s <port>\n", argv[0]);
    exit(0);
  }

  char *ip = "127.0.0.1";
  int port = atoi(argv[1]);

  int sockfd;
  int acksockfd;

  struct sockaddr_in server_addr, client_addr, server_ack_addr, client_ack_addr;
  char buffer[1024];
  socklen_t addr_size, client_ack_addr_size;
  int n;
  pid_t childpid;

  Request req;
  Response res;
  Ack ack;

  /* Initialize the random number generator */
  srand(getpid());

  /* Create the socket that is going to listen to clients requests */
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0)
  {
    perror("[-]main socket error");
    exit(1);
  }

  /* Create the socket that is going to listen for the clients ack */
  acksockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (acksockfd < 0)
  {
    perror("[-]ack socket error");
    exit(1);
  }

  memset(&server_addr, '\0', sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  server_addr.sin_addr.s_addr = inet_addr(ip);

  memset(&server_ack_addr, '\0', sizeof(server_ack_addr));
  server_ack_addr.sin_family = AF_INET;
  server_ack_addr.sin_port = htons(3000);
  server_ack_addr.sin_addr.s_addr = inet_addr(ip);

  n = bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (n < 0)
  {
    perror("[-]main socket bind error");
    exit(1);
  }

  n = bind(acksockfd, (struct sockaddr *)&server_ack_addr, sizeof(server_ack_addr));
  if (n < 0)
  {
    perror("[-]ack socket bind error");
    exit(1);
  }

  addr_size = sizeof(client_addr);

  while (1)
  {
    recvfrom(sockfd, &req, sizeof(Request), 0, (struct sockaddr *)&client_addr, &addr_size);
    printf("[+]Client IP adress: %s and port: %d with PID %d send %d as payload.\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), req.client_pid, req.data);

    res.size = req.data;
    for (int i = 0; i < res.size; i++)
    {
      res.data[i] = (rand() % (UPPER - LOWER + 1)) + LOWER;
    }

    res.server_pid = getppid();
    res.server_child_pid = getpid();

    if ((childpid = fork()) == 0)
    {
      sendto(sockfd, &res, sizeof(res), 0, (struct sockaddr *)&client_addr, sizeof(client_addr));
      printf("[+]Data sent to the client %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

      recvfrom(acksockfd, &ack, sizeof(Ack), 0, (struct sockaddr *)&client_ack_addr, &client_ack_addr_size);
      if (ack.confirmation == 1)
      {
        printf("[+]Received ack from %s:%d\n", inet_ntoa(client_ack_addr.sin_addr), ntohs(client_ack_addr.sin_port));
      } else {
        printf("[-]Error occured while  receiving the ack\n");
        exit(1);
      }
    }
  }

  close(sockfd);
  close(acksockfd);

  return 0;
}
