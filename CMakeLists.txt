cmake_minimum_required(VERSION 3.21)
project(TMsgNow)

set(CMAKE_CXX_STANDARD 14)

# add extra include directories
include_directories(/opt/homebrew/Cellar/boost/1.78.0_1/include)

add_executable(TMsgNow
        sample_basic/sample_accepting_connections/acceptor_socket_listening_connections.cpp
        sample_basic/sample_berkeley_sockets_api/active_tcp_socket.cpp
        sample_basic/sample_berkeley_sockets_api/active_udp_socket.cpp
        sample_basic/sample_berkeley_sockets_api/passive_tcp_socket.cpp
        sample_basic/sample_binding_socket_to_endpoint/acceptor_socket_bind_endpoint.cpp
        sample_basic/sample_binding_socket_to_endpoint/udp_active_socket_bind_endpoint.cpp
        sample_basic/sample_connect_socket/tcp_client_socket_connect_server.cpp
        sample_basic/sample_connect_socket/tcp_client_socket_connect_server_with_dns.cpp
        sample_basic/sample_dns/dns_tcp_v6.cpp
        sample_basic/sample_dns/dns_udp_protocol.cpp
        sample_basic/sample_endpoint/client.cpp
        sample_basic/sample_endpoint/server.cpp
        sample_implementing_client/asynchronous_tcp_client.cpp
        sample_implementing_client/synchronous_tcp_client.cpp
        sample_implementing_client/synchronous_udp_client.cpp
        sample_io_operations/sample_fixed_length_io_buffers/buffer_for_input_operation.cpp
        sample_io_operations/sample_fixed_length_io_buffers/buffer_for_output_operation.cpp
        sample_io_operations/sample_shutting_down_closing_socket/sync_client.cpp
        sample_io_operations/sample_shutting_down_closing_socket/sync_server.cpp
        sample_io_operations/sample_stream_oriented_io_buffers/stream_buffer_object.cpp
        sample_io_operations/sample_tcp_socket_asynchronously/canceling_asyn_operations_deprecated.cpp
        sample_io_operations/sample_tcp_socket_asynchronously/read_data_from_tcp_socket_asyn_client.cpp
        sample_io_operations/sample_tcp_socket_asynchronously/write_data_to_tcp_socket_asyn_client.cpp
        sample_io_operations/sample_tcp_socket_synchronously/read_data_from_tcp_socket_syn_client.cpp
        sample_io_operations/sample_tcp_socket_synchronously/write_data_to_tcp_socket_syn_client.cpp)
