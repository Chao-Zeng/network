#ifndef _SOCKET_EVENTS_HANDLER_H_
#define _SOCKET_EVENTS_HANDLER_H_

class socket_events_handler
{
public:
    virtual void handle_data() = 0;
    virtual void handle_error() = 0;
    virtual void handle_hang_up() = 0;
    virtual void handle_peer_closed() = 0;
    virtual void handle_urgent_data() {}

protected:
    socket_events_handler() {}
    virtual ~socket_events_handler() {}
};

#endif // _SOCKET_EVENTS_HANDLER_H_
