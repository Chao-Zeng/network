#ifndef _EPOLL_EVENTS_HANDLER_H_
#define _EPOLL_EVENTS_HANDLER_H_

// platform header
#include <sys/epoll.h>

// C C++ header
#include <stdint.h>

class epoll_events_handler
{
public:
    virtual void handle_read() = 0;
    virtual void handle_write() = 0;    
    virtual void handle_error() = 0;
    virtual void handle_hang_up() = 0;
    virtual void handle_peer_closed() {}
    virtual void handle_urgent_data() {}
    
    void handle_events(uint32_t events)
    {
        if (events & EPOLLIN)
        {
            handle_read();
        }

        if (events & EPOLLOUT)
        {
            handle_write();
        }

        if (events & EPOLLRDHUP)
        {
            handle_peer_closed();
        }

        if (events & EPOLLPRI)
        {
            handle_urgent_data();
        }

        if (events & EPOLLERR)
        {
            handle_error();
        }

        if (events & EPOLLHUP)
        {
            handle_hang_up();
        }
    }

protected:
    epoll_events_handler() {}
    virtual ~epoll_events_handler() {}
};

#endif // _EPOLL_EVENTS_HANDLER_H_
