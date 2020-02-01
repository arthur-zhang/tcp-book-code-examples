
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUF_SIZE 256

int main(void) {
  int sock_fd, connect_fd;
  char buffer[BUF_SIZE];
  struct sockaddr_in serv_addr, cli_addr;
  int cli_addr_len = sizeof(cli_addr);
  int n;

  sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  int optval = 1;

  setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
  setsockopt(sock_fd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
  bzero((char *)&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(9090);

  int ret = bind(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
  if (ret < 0) {
    printf("bind error, code is %d\n", ret);
    exit(1);
  }

  listen(sock_fd, 5);

  while (1) {
    connect_fd = accept(sock_fd, (struct sockaddr *)&cli_addr, &cli_addr_len);
    printf("process new request\n");
    n = read(connect_fd, buffer, BUF_SIZE);
    write(connect_fd, buffer, n);
    close(connect_fd);
  }
  return 0;
}