cmake_minimum_required(VERSION 3.20.0-rc2)

add_executable(list_adapters list_adapters.c)

add_executable(tcp_client tcp_client.c)

add_executable(udp_client udp_client.c)

add_executable(udp_client_broadcast udp_client_broadcast.c)

add_executable(unix_tcp_client unix_tcp_client.c)

add_executable(unix_tcp_server unix_tcp_server.c)

add_executable(unix_udp_client unix_udp_client.c)

add_executable(unix_udp_client_connected unix_udp_client_connected.c)

add_executable(unix_udp_server unix_udp_server.c)

add_executable(unix_udp_server_abstract unix_udp_server_abstract.c)

add_executable(unix_udp_server_connected unix_udp_server_connected.c)