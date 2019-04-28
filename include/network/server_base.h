#ifndef _SERVER_BASE_H_
#define _SERVER_BASE_H_

// C C++ header
#include <map>
#include <vector>

//forward declaration
class acceptor;
class epoll_reactor;

class server_base
{
public:
    epoll_reactor* reactor();

    bool start();

    void stop();

    virtual void handle_new_connection(int peer) = 0;

protected:
    server_base(epoll_reactor* reactor, short port);
    server_base(epoll_reactor* reactor, std::vector<short> ports);
    virtual ~server_base();

private:
    // delete copy constructor
    server_base(const server_base&);
    // delete copy assignment
    server_base& operator= (const server_base&);

private:
    epoll_reactor* m_reactor;
    std::vector<short> m_listen_ports;
    std::map<short, acceptor*> m_acceptors;
};

#endif // _SERVER_BASE_H_
