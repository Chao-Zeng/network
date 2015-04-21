#include "tcp_socket.h"

#include <unistd.h>

#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "epoll_reactor.h"
#include "socket_functions.h"
#include "socket_events_handler.h"

//this size should be greater than tcp buffer to guarantee read/write all data in one EPOLLIN/EPOLLOUT events
static const size_t buff_size = 1024 * 4;

tcp_socket::tcp_socket(epoll_reactor* reactor, int sock, socket_events_handler* handler)
: m_reactor(reactor), m_socket(sock), m_handler(handler)
{
    set_nonblocking(m_socket);
    m_reactor->add(m_socket, EPOLLIN, this);
}

tcp_socket::~tcp_socket()
{
    close();
}

int tcp_socket::handle() const
{
    return m_socket;
}

void tcp_socket::close()
{
    m_reactor->remove(m_socket, EPOLLIN | EPOLLOUT, this);
    ::close(m_socket);
    m_read_cache.clear();
    m_write_cache.clear();
}

size_t tcp_socket::get_read_data_length()
{
    return m_read_cache.get_length();
}

bool tcp_socket::get_data(char* buff, size_t length)
{
    return m_read_cache.get_data((uint8_t*)buff, length);
}

bool tcp_socket::read_data(char* buff, size_t length)
{
    bool ret = m_read_cache.get_data((uint8_t*)buff, length);
    
    if (ret)
    {
        m_read_cache.erase_data(length);
    }

    return ret;
}

bool tcp_socket::write_data(const char* buff, size_t length)
{
    bool ret = m_write_cache.push_data((uint8_t*)buff, length);

    if (ret)
    {
        m_reactor->modify(m_socket, EPOLLIN | EPOLLOUT, this);
        m_reactor->interrupt();
    }

    return ret;
}

void tcp_socket::handle_read()
{
    char read_buff[buff_size] = { 0 };

    int bytes_read = read_all(m_socket, read_buff, buff_size);

    if (bytes_read < 0)
    {
        m_handler->handle_error();
        return;
    }
    else if (0 == bytes_read)
    {
        m_handler->handle_peer_closed();
        return;
    }
    else
    {
        m_read_cache.push_data((uint8_t*)read_buff, bytes_read);

        m_handler->handle_data();
    }

    return;
}

void tcp_socket::handle_write()
{
    size_t data_length = m_write_cache.get_length();
    if (0 == data_length)
    {
        m_reactor->modify(m_socket, EPOLLIN, this);
        m_reactor->interrupt();
        return;
    }

    char* write_buff = (char*)malloc(data_length);

    if (NULL == write_buff)
    {
        return;
    }

    if (!m_write_cache.get_data((uint8_t*)write_buff, data_length))
    {
        free(write_buff);
        return;
    }

    int bytes_write = write_all(m_socket, write_buff, data_length);

    free(write_buff);
    write_buff = NULL;

    if (bytes_write < 0 || 0 == bytes_write)
    {
        m_handler->handle_error();
        return;
    }

    m_write_cache.erase_data(bytes_write);
        
    if (0 == m_write_cache.get_length())
    {
        m_reactor->modify(m_socket, EPOLLIN, this);
        m_reactor->interrupt();
        return;
    }

    return;
}

void tcp_socket::handle_error()
{
    m_handler->handle_error();
}

void tcp_socket::handle_hang_up()
{
    m_handler->handle_hang_up();
}


