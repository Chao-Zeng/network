#include "server_base.h"

// C C++ header
#include <new>

//project header
#include "epoll_reactor.h"
#include "acceptor.h"



server_base::server_base(epoll_reactor* reactor, short port) : m_reactor(reactor)
{
    m_acceptors[port] = new acceptor(port, this);
}

server_base::server_base(epoll_reactor* reactor, std::vector<short> ports) : m_reactor(reactor)
{
    typedef std::vector<short>::iterator ite;
    for (ite it = ports.begin(); it != ports.end(); ++it)
    {
        m_acceptors[*it] = new acceptor(*it, this);
    }
}

server_base::~server_base()
{
    m_reactor = NULL;

    typedef std::map<short, acceptor*>::iterator ite;
    for (ite it = m_acceptors.begin(); it != m_acceptors.end(); ++it)
    {
        delete it->second;
    }

    m_acceptors.clear();
}

epoll_reactor* server_base::reactor()
{
    return m_reactor;
}

bool server_base::start()
{
    typedef std::map<short, acceptor*>::iterator ite;
    for (ite it = m_acceptors.begin(); it != m_acceptors.end(); ++it)
    {
        if (!it->second->start())
        {
            stop();
            return false;
        }
    }

    return true;
}

void server_base::stop()
{
    typedef std::map<short, acceptor*>::iterator ite;
    for (ite it = m_acceptors.begin(); it != m_acceptors.end(); ++it)
    {
        it->second->stop();
    }
}
