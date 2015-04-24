#include <stdio.h>
#include <stdlib.h>

#include "test_client.h"
#include "../network/socket_functions.h"
#include "../network/epoll_reactor.h"

int main(int argc, char* argv[])
{
    if (3 != argc)
    {
        printf("usage: client server_ip server_port\n");
        return 0;
    }

    char* server_ip = argv[1];
    short server_port = atoi(argv[2]);

    epoll_reactor reactor;

    int client_sock = connect(server_ip, server_port);

    if (client_sock < 0)
    {
        printf("connect server %s:%d failed\n", server_ip, server_port);
        return 0;
    }

    printf("connected to server %s:%d\n", get_peer_ip(client_sock).c_str(), get_peer_port(client_sock));

    test_client client(&reactor, client_sock);

    if (!reactor.run())
    {
        return 0;
    }

    while (1)
    {
        char data[1024];
        memset(data, 0, 1024);
        gets(data);

        client.send_data(data, strlen(data));
    }

    return 0;
}