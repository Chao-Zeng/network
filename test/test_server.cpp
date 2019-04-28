#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <iostream>

#include "test_server.h"
#include "network/epoll_reactor.h"

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("usage: server port\n");
        return 0;
    }      

    epoll_reactor reactor;
    
    test_server server(&reactor, atoi(argv[1]));

    if (!server.start())
    {
        return 0;
    }

    if (!reactor.run())
    {
        return 0;
    }

    
    while (1)
    {
        char data[1024];
        memset(data, 0, 1024);
        fgets(data, sizeof(data), stdin);

        server.m_client->send_data(data, strlen(data));
    }

    return 0;
}
