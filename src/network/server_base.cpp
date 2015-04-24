#include "server_base.h"

// C C++ header
#include <new>

//project header
#include "epoll_reactor.h"
#include "acceptor.h"



server_base::server_base(epoll_reactor* reactor, short port) : m_reactor(reactor)
{
    m_listen_ports.push_back(port);
}

server_base::server_base(epoll_reactor* reactor, std::vector<short> ports) : m_reactor(reactor), m_listen_ports(ports)
{
    
}

server_base::~server_base()
{
    m_reactor = NULL;
    m_listen_ports.clear();
    stop();
}

epoll_reactor* server_base::reactor()
{
    return m_reactor;
}

bool server_base::start()
{
    typedef std::vector<short>::iterator ite;
    for (ite it = m_listen_ports.begin(); it != m_listen_ports.end(); ++it)
    {
        acceptor* _acceptor = new acceptor(*it, this);

        if (!_acceptor->start())
        {
            delete _acceptor;
            stop();
            return false;
        }

        m_acceptors[*it] = _acceptor;
    }

    return true;
}

void server_base::stop()
{
    typedef std::map<short, acceptor*>::iterator ite;
    for (ite it = m_acceptors.begin(); it != m_acceptors.end(); ++it)
    {
        if (NULL != it->second)
        {
            it->second->stop();
            delete it->second;
            it->second = NULL;
        }
    }

    m_acceptors.clear();
}
