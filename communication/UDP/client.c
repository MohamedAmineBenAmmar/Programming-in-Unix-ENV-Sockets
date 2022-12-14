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

  /* Initializing the ip address and the port */
  char *ip = "127.0.0.1";
  int port = atoi(argv[1]);

  /* Declare the needed variables */
  int sockfd;
  struct sockaddr_in addr, ackaddr;
  char buffer[1024];
  socklen_t addr_size;

  Request req;
  Response res;
  Ack ack;
  int data;

  /* Init the random number generator */
  srand(getpid());

  /* Creating the client socket */
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0)
  {
    printf("[-]Error in the client socket creation.\n");
    exit(1);
  }
  printf("[+]Client Socket is created.\n");

  /* Configuring server socket data */
  memset(&addr, '\0', sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = inet_addr(ip);

  /* Configuring server ack socket data */
  memset(&ackaddr, '\0', sizeof(ackaddr));
  ackaddr.sin_family = AF_INET;
  ackaddr.sin_port = htons(3000);
  ackaddr.sin_addr.s_addr = inet_addr(ip);

  /* Generating a random number */
  data = (rand() % (UPPER - LOWER + 1)) + LOWER;
  printf("Client random generated number: %d \n", data);

  /* Preparing the request */
  req.data = data;
  req.client_pid = getpid();

  /* Send a request to the server */
  sendto(sockfd, &req, sizeof(req), 0, (struct sockaddr *)&addr, sizeof(addr));
  printf("[+]Data sent: %d\n", data);

  addr_size = sizeof(addr);

  /* Receiving response from the server */
  recvfrom(sockfd, &res, sizeof(Response), 0, (struct sockaddr *)&addr, &addr_size);
  printf("Server responded with:\t");
  for (int i = 0; i < res.size; i++)
  {
    printf("%d ", res.data[i]);
  }
  printf("\n");

  /* Preparing the ack */
  ack.client_pid = getpid();
  ack.confirmation = 1;

  /* Sending ack to the server */
  sendto(sockfd, &ack, sizeof(ack), 0, (struct sockaddr *)&ackaddr, sizeof(ackaddr));

  close(sockfd);
  printf("[-]Disconnected from server.\n");
  return 0;
}
