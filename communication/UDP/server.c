#include "../../global/libs.h"

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

  int childs = 0;

  struct sockaddr_in server_addr, client_addr, server_ack_addr, client_ack_addr;
  char buffer[1024];
  socklen_t addr_size, client_ack_addr_size;
  int n;
  pid_t childpid;

  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0)
  {
    perror("[-]socket error");
    exit(1);
  }

  acksockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (acksockfd < 0)
  {
    perror("[-]socket error");
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
    perror("[-]bind error");
    exit(1);
  }

  n = bind(acksockfd, (struct sockaddr *)&server_ack_addr, sizeof(server_ack_addr));
  if (n < 0)
  {
    perror("[-]bind error");
    exit(1);
  }

  while (1)
  {
    bzero(buffer, 1024);
    addr_size = sizeof(client_addr);
    recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr *)&client_addr, &addr_size);
    printf("[+]Data recv: %s\n", buffer);

    if ((childpid = fork()) == 0)
    {

      printf("Im communicting with \n");
      printf("Data : adress %s port: %d.\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
      bzero(buffer, 1024);
      strcpy(buffer, "Welcome to the UDP Server.");
      sendto(sockfd, buffer, 1024, 0, (struct sockaddr *)&client_addr, sizeof(client_addr));
      printf("[+]Data send: %s\n", buffer);

      recvfrom(acksockfd, buffer, 1024, 0, (struct sockaddr *)&client_ack_addr, &client_ack_addr_size);
      printf("jeni ack men 3and \n");
      printf("Data : adress %s port: %d\n", inet_ntoa(client_ack_addr.sin_addr), ntohs(client_ack_addr.sin_port));
    }
  }

  return 0;
}
