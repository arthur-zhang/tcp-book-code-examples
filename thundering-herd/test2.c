#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#define PROCESS_NUM 10
#define MAXEVENTS 64
static int create_and_bind(char *port) {
  int fd = socket(PF_INET, SOCK_STREAM, 0);
  struct sockaddr_in serveraddr;
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serveraddr.sin_port = htons(atoi(port));

  int optval = 1;
  setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

  if (bind(fd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0) {
    perror("bind failed");
    exit(1);
  }
  return fd;
}
static int make_socket_non_blocking(int sfd) {
  int flags, s;

  flags = fcntl(sfd, F_GETFL, 0);
  if (flags == -1) {
    perror("fcntl");
    return -1;
  }

  flags |= O_NONBLOCK;
  s = fcntl(sfd, F_SETFL, flags);
  if (s == -1) {
    perror("fcntl");
    return -1;
  }

  return 0;
}

int main(int argc, char *argv[]) {
  printf("here\n");
  int sock_fd, s;
  int epoll_fd;
  struct epoll_event event;
  struct epoll_event *events;

  sock_fd = create_and_bind("9090");
  if (sock_fd == -1)
    abort();

  s = make_socket_non_blocking(sock_fd);
  if (s == -1)
    abort();

  s = listen(sock_fd, SOMAXCONN);
  if (s == -1) {
    perror("listen");
    exit(1);
  }

  // 创建一个 epoll 实例, 从 Linux 2.6.8 开始 size 参数被忽略
  epoll_fd = epoll_create(1);
  if (epoll_fd == -1) {
    perror("epoll_create");
    exit(1);
  }
  printf("epoll_fd: %d\n", epoll_fd);

  event.data.fd = sock_fd;

  // EPOLLIN 表示文件描述符可读
//  event.events = EPOLLIN | EPOLLEXCLUSIVE; //  只关心 input events
  event.events = EPOLLIN;
//  event.events |= EPOLLET;

  // 第一个参数表示 epoll 句柄
  // 第二个参数 EPOLL_CTL_ADD 表示向 epoll 实例注册事件
  // 第三个参数 sock_fd 表示注册事件的文件描述符句柄
  // 第四个参数 event 表示注册的事件类型
  // 返回值：返回 0 则成功，-1 表示失败
  if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock_fd, &event) == -1) {
    perror("epoll_ctl");
    exit(1);
  }

  events = calloc(MAXEVENTS, sizeof(event));
  for (int i = 0; i < 5; i++) {
    if (fork() == 0) {
      while (1) {
        // 第一个参数 epfd 表示 epoll 句柄
        // 第二个参数 events 表示事件的类型
        // 第三个参数 maxevents 表示返回的最大事件值
        // 第四个参数 timeout 表示 epoll_wait 阻塞调用的超时时间
        // 如果返回值是一个大于 0 的数字，表示事件的个数，返回 0 表示超时时间到，返回 -1 表示出错
        int n = epoll_wait(epoll_fd, events, MAXEVENTS, -1);

        printf("return from epoll_wait: %d, pid is %d\n", n, getpid());
        sleep(2);
        for (int j = 0; j < n; j++) {
          if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) ||
              (!(events[i].events & EPOLLIN))) {
            fprintf(stderr, "epoll err pid: %d, %d, %d, %d\n",
                    getpid(),
                    events[i].events & EPOLLERR,
                    events[i].events & EPOLLHUP,
                    events[i].events & EPOLLIN
            );
            close(events[i].data.fd);
            continue;
          } else if (sock_fd == events[j].data.fd) {
            struct sockaddr sock_addr;
            socklen_t sock_len;
            int conn_fd;
            sock_len = sizeof(sock_addr);
            conn_fd = accept(sock_fd, &sock_addr, &sock_len);
            if (conn_fd == -1) {
              printf("accept failed, pid is %d\n", getpid());
              break;
            }
            printf("accept success, pid is %d\n", getpid());
            close(conn_fd);
          }
        }
      }
    }
  }
  int status;
  wait(&status);
  free(events);
  close(sock_fd);
  return EXIT_SUCCESS;
}