#ifndef _ACCEPTOR_H_
#define _ACCEPTOR_H_

#include "network/epoll_events_handler.h"

class server_base;

class acceptor : public epoll_events_handler
{
public:
    acceptor(short port, server_base* hander);
    ~acceptor();

    bool start();

    void stop();

private:
    void handle_read();
    void handle_write();
    void handle_error();
    void handle_hang_up();

private:
    // delete copy constructor
    acceptor(const acceptor&);
    // delete copy assignment
    acceptor& operator= (const acceptor&);

private:
    short m_listen_port;
    server_base* m_accept_handler;
    int m_accept_socket;    
};

#endif // _ACCEPTOR_H_
