cmake_minimum_required(VERSION 3.15)
project(tcp_examples C)

set(CMAKE_C_STANDARD 99)

add_executable(tcp_examples reuse_port/reuse_port_test.c reuse_port/test.c thundering-herd/test1.c thundering-herd/test2.c ephemeral-port/test1.c tcp-echo-server/tcp-echo-server.c)