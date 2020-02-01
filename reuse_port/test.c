//
// Created by 张亚 on 2020/1/17.
//
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
int main() {
  struct sockaddr_in serv_addr;
  int sock_fd = socket(AF_INET, SOCK_STREAM, 0);

//  int optval = 1;
//  setsockopt(sock_fd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
  bzero((char *)&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons(9090);
  int ret = bind(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
  if (ret < 0) {
	printf("bind error, code is %d\n", ret);
	exit(1);
  }
  sleep(-1);
  return 0;
}