#include "test_server.h"
#include <stdio.h>
#include <stdlib.h>
#include "epoll_reactor.h"
#include <unistd.h>

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

    pause();

    return 0;
}