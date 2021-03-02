cmake_minimum_required(VERSION 3.20.0-rc2)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads)

add_executable(tcp_server tcp_server.c)
target_link_libraries(tcp_server ${CMAKE_THREAD_LIBS_INIT})

add_executable(udp_server udp_server.c)
target_link_libraries(udp_server ${CMAKE_THREAD_LIBS_INIT})

add_executable(multithread_tcp_server multithread_tcp_server.cpp)
target_link_libraries(multithread_tcp_server ${CMAKE_THREAD_LIBS_INIT})