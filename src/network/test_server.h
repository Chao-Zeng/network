#ifndef _TEST_SERVER_H_
#define _TEST_SERVER_H_

#include <unistd.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "epoll_reactor.h"
#include "server_base.h"
#include "socket_functions.h"
#include "epoll_events_handler.h"
#include "tcp_socket.h"
#include "socket_events_handler.h"

class tcp_connection : socket_events_handler
{
public:
    tcp_connection(epoll_reactor* reactor, int sockfd) : m_tcp_socket(reactor, sockfd, this)
    {
        
    }
    
    void handle_data()
    {        
        size_t data_length = m_tcp_socket.get_read_data_length();

        char *buff = (char *)malloc(data_length);
        if (NULL == buff)
        {
            return;
        }
        
        if (!m_tcp_socket.read_data(buff, data_length))
        {
            printf("read data error\n");
        }
        else
        {
            printf("receive data:%s\n", buff);
        }
    }

    void send_data(const char* str, size_t len)
    {
        m_tcp_socket.write_data(str, len);
    }
        
    void handle_error()
    {
        printf("tcp socket error %d:%s\n", errno, strerror(errno));
    }

    void handle_hang_up()
    {
        printf("socket hang up\n");
    }

    void handle_peer_closed()
    {
        printf("peer closed\n");
    }

private:
    tcp_socket m_tcp_socket;
};

class test_server : public server_base
{
public:
    test_server(epoll_reactor* reactor, short port) : server_base(reactor, port), m_client(NULL)
    {

    }

    void handle_new_connection(int peer)
    {
        printf("accept connection local %s:%d, peer %s:%d\n", get_local_ip(peer).c_str(), get_local_port(peer),
            get_peer_ip(peer).c_str(), get_peer_port(peer));

       m_client = new tcp_connection(reactor(), peer);

        char* str = "welcome login";
        m_client->send_data(str, strlen(str));
    }

public:
    tcp_connection* m_client;
};


#endif // _TEST_SERVER_H_
