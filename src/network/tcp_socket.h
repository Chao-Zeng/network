#ifndef _TCP_SOCKET_H_
#define _TCP_SOCKET_H_

#include <stddef.h>

#include "epoll_events_handler.h"
#include "cache_buffer.h"

// forward declaration
class epoll_reactor;
class socket_events_handler;

class tcp_socket : public epoll_events_handler
{
public:
    tcp_socket(epoll_reactor* reactor, int sock, socket_events_handler* handler);
    ~tcp_socket();
    
    int handle() const;

    void close();

    size_t get_read_data_length();

    //get data from read cache buffer, data still in cache buffer
    bool get_data(char* buff, size_t length);

    //get and erase data from read cache buffer
    bool read_data(char* buff, size_t length);

    bool write_data(const char* buff, size_t length);

private:
    void handle_read();
    void handle_write();
    void handle_error();
    void handle_hang_up();
    //if watch EPOLLRDHUP, need implement
    void handle_peer_closed() {}
    //if watch EPOLLPRI, need implement
    void handle_urgent_data() {}

private:
    // delete copy constructor
    tcp_socket(const tcp_socket&);
    // delete copy assignment
    tcp_socket& operator= (const tcp_socket&);

private:
    epoll_reactor* m_reactor;
    int m_socket;
    socket_events_handler* m_handler;
    cache_buffer m_read_cache;
    cache_buffer m_write_cache;
};

#endif // _TCP_SOCKET_H_
