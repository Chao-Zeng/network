#ifndef _TEST_SERVER_H_
#define _TEST_SERVER_H_

#include <unistd.h>

#include <stdio.h>

#include "epoll_reactor.h"
#include "server_base.h"
#include "socket_functions.h"
#include "epoll_events_handler.h"

class tcp_connection : public epoll_events_handler
{
public:
    tcp_connection(int sockfd) : m_sockfd(sockfd)
    {

    }

    void handle_read()
    {
        int buff_size = 2048;
        char buff[2048] = {0};

        int ret = read_all(m_sockfd, buff, buff_size);

        if (0 == ret)
        {
            printf("peer closed connection\n");
        }
        else if (ret < 0)
        {
            printf("read data error\n");
        }
        else
        {
            printf("receive data:%s\n", buff);
        }
    }

    void handle_peer_closed()
    {
        printf("peer closed\n");
    }

    void handle_write() {}
    void handle_error() {}
    void handle_hang_up() {}

private:
    int m_sockfd;
};

class test_server : public server_base
{
public:
    test_server(epoll_reactor* reactor, short port) : server_base(reactor, port)
    {

    }

    void handle_new_connection(int peer)
    {
        printf("accept connection local %s:%d, peer %s:%d\n", get_local_ip(peer).c_str(), get_local_port(peer),
            get_peer_ip(peer).c_str(), get_peer_port(peer));

        set_nonblocking(peer);

        tcp_connection* new_connection = new tcp_connection(peer);

        reactor()->add(peer, EPOLLIN | EPOLLRDHUP, new_connection);
    }
};


#endif // _TEST_SERVER_H_
