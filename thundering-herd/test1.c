#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
int main(void) {
  int listenfd, connfd;
  socklen_t clilen;
  struct sockaddr_in cliaddr, servaddr;
  listenfd = socket(AF_INET, SOCK_STREAM, 0);
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl (INADDR_ANY);
  servaddr.sin_port = htons (9090);
  bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
  listen(listenfd, 5);
  clilen = sizeof(cliaddr);

  for (int i = 0; i < 4; ++i) {
	if ((fork()) == 0) {
	  // 子进程
	  printf("child pid: %d\n", getpid());
	  while (1) {
		connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
		sleep(2);
		printf("processing, pid is %d\n", getpid());
	  }
	}
  }

  sleep(-1);

  return 1;

}
