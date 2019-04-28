#include "network/acceptor.h"

//platform header
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>

//C C++ header
#include <string.h>

//project header
#include "network/server_base.h"
#include "network/epoll_reactor.h"

acceptor::acceptor(short port, server_base* hander) : m_listen_port(port), m_accept_handler(hander), m_accept_socket(-1)
{

}

acceptor::~acceptor()
{

}

bool acceptor::start()
{
    m_accept_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (m_accept_socket < 0)
    {
        return false;
    }

    //set reuse address
    int on = 1;
    if (0 != setsockopt(m_accept_socket, SOL_SOCKET, SO_REUSEADDR, (const void*)&on, sizeof(on)))
    {
        close(m_accept_socket);
        m_accept_socket = -1;
        return false;
    }

    //set linger
    linger ling;
    ling.l_onoff = 1;
    ling.l_linger = 0;
    if (0 != setsockopt(m_accept_socket, SOL_SOCKET, SO_LINGER, (const char*)&ling, sizeof(linger)))
    {
        close(m_accept_socket);
        m_accept_socket = -1;
        return false;
    }
    

    //set non-blocking
    if (0 != fcntl(m_accept_socket, F_SETFL, fcntl(m_accept_socket, F_GETFL, 0) | O_NONBLOCK))
    {
        close(m_accept_socket);
        m_accept_socket = -1;
        return false;
    }

    //bind address
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(m_listen_port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (0 != bind(m_accept_socket, (struct sockaddr *)&addr, sizeof(addr)))
    {
        close(m_accept_socket);
        m_accept_socket = -1;
        return false;
    }

    //add to reactor
    if (0 != m_accept_handler->reactor()->add(m_accept_socket, EPOLLIN, this))
    {
        close(m_accept_socket);
        m_accept_socket = -1;
        return false;
    }

    //start listen
    if (0 != listen(m_accept_socket, SOMAXCONN))
    {
        m_accept_handler->reactor()->remove(m_accept_socket, EPOLLIN, this);
        close(m_accept_socket);
        m_accept_socket = -1;
        return false;
    }

    return true;
}

void acceptor::stop()
{
    m_accept_handler->reactor()->remove(m_accept_socket, EPOLLIN, this);
    close(m_accept_socket);
    m_accept_socket = -1;
}

void acceptor::handle_read()
{
    struct sockaddr_in peer_addr;
    socklen_t len = sizeof(peer_addr);
    int peer_socket = accept(m_accept_socket, (struct sockaddr*)&peer_addr, &len);

    if (peer_socket < 0)
    {
        return;
    }

    m_accept_handler->handle_new_connection(peer_socket);
}

void acceptor::handle_write()
{
    // we never add EPOLLOUT
}

void acceptor::handle_error()
{
    // error handle
}

void acceptor::handle_hang_up()
{
    // hang up handle
}

