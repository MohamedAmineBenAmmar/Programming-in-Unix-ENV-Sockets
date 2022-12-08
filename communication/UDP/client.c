#include "../../global/libs.h"

int main(int argc, char **argv)
{
  int x;

  if (argc != 2)
  {
    printf("Usage: %s <port>\n", argv[0]);
    exit(0);
  }

  char *ip = "127.0.0.1";
  int port = atoi(argv[1]);

  int sockfd;
  struct sockaddr_in addr, ackaddr;
  char buffer[1024];
  socklen_t addr_size;

  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  memset(&addr, '\0', sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = inet_addr(ip);

  memset(&ackaddr, '\0', sizeof(ackaddr));
  ackaddr.sin_family = AF_INET;
  ackaddr.sin_port = htons(3000);
  ackaddr.sin_addr.s_addr = inet_addr(ip);

  bzero(buffer, 1024);
  strcpy(buffer, "Hello, World!");
  sendto(sockfd, buffer, 1024, 0, (struct sockaddr *)&addr, sizeof(addr));
  printf("[+]Data send: %s\n", buffer);

  bzero(buffer, 1024);
  addr_size = sizeof(addr);
  recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr *)&addr, &addr_size);
  printf("[+]Data recv: %s\n", buffer);

  //  printf("the new socket created by the server :%d\n", ntohs(tmpaddr.sin_port));
  printf("the old socket created by the server :%d\n", ntohs(addr.sin_port));

  printf("da5el value\n");
  scanf("%d", &x);
  sendto(sockfd, "yoooo hhh", 1024, 0, (struct sockaddr *)&ackaddr, sizeof(ackaddr));

  return 0;
}
