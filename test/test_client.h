#ifndef _TEST_CLIENT_H_
#define _TEST_CLIENT_H_

#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "../network/socket_events_handler.h"
#include "../network/tcp_socket.h"

class epoll_reactor;

class test_client : private socket_events_handler
{
public:
    test_client(epoll_reactor* reactor, int sock) : m_reactor(reactor), m_tcp_socket(reactor, sock, this)
    {

    }

    ~test_client()
    {

    }

    void send_data(const char* str, size_t len)
    {
        m_tcp_socket.write_data(str, len);
    }

private:
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
    
    void handle_error()
    {
        printf("tcp socket error %d:%s\n", errno, strerror(errno));
    }

    void handle_hang_up()
    {
        printf("hang up\n");
    }

    void handle_peer_closed()
    {
        printf("peer closed\n");
    }

private:
    epoll_reactor* m_reactor;
    tcp_socket m_tcp_socket;
};

#endif // _TEST_CLIENT_H_
