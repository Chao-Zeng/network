#include "epoll_reactor.h"

// platform header
#include <sys/epoll.h>
#include <unistd.h>
#include <signal.h>

// C C++ header
#include <stdint.h>
#include <stddef.h>

// project header
#include "epoll_events_handler.h"

epoll_reactor::epoll_reactor(int size) : m_epoll_fd(epoll_create(size)), m_thread_id(0), m_run(false)
{
    m_pipe_fd[0] = -1;
    m_pipe_fd[1] = -1;
}

epoll_reactor::~epoll_reactor()
{
    close(m_epoll_fd);
}

int epoll_reactor::add(int fd, uint32_t events, epoll_events_handler* events_handler)
{
    return operate(EPOLL_CTL_ADD, fd, events, events_handler);
}

int epoll_reactor::modify(int fd, uint32_t events, epoll_events_handler* events_handler)
{
    return operate(EPOLL_CTL_MOD, fd, events, events_handler);
}

int epoll_reactor::remove(int fd, uint32_t events, epoll_events_handler* events_handler)
{
    return operate(EPOLL_CTL_DEL, fd, events, events_handler);
}

void epoll_reactor::interrupt()
{
    write(m_pipe_fd[1], "i", 1);
}

bool epoll_reactor::run()
{
    if (m_run)
    {
        return true;
    }

    if (!init_interrupter())
    {
        return false;
    }

    m_run = true;

    if (-1 == pthread_create(&m_thread_id, NULL, poll_thread, (void*)this))
    {
        m_run = false;
        return false;
    }

    signal(SIGPIPE, SIG_IGN);

    return true;
}

void epoll_reactor::stop()
{
    if (!m_run)
    {
        return;
    }

    m_run = false;
    interrupt();

    pthread_join(m_thread_id, NULL);

    destroy_interrupter();
}

void* epoll_reactor::poll_thread(void* arg)
{
    epoll_reactor* p_this = (epoll_reactor*)arg;

    struct epoll_event events_array[MAX_EVENTS];

    while (p_this->m_run)
    {
        int nfds = epoll_wait(p_this->m_epoll_fd, events_array, MAX_EVENTS, -1);

        if (-1 == nfds)
        {
            continue;
        }

        for (int i = 0; i < nfds; ++i)
        {
            if (events_array[i].data.fd == p_this->m_pipe_fd[0])
            {
                int tmp;
                read(p_this->m_pipe_fd[0], &tmp, sizeof(tmp));
                continue;
            }

            epoll_events_handler* handler = (epoll_events_handler*)(events_array[i].data.ptr);
            handler->handle_events(events_array[i].events);
        }
    }

    return NULL;
}

bool epoll_reactor::init_interrupter()
{
    if (-1 == pipe(m_pipe_fd))
    {
        return false;
    }

    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = m_pipe_fd[0];

    if (-1 == epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, m_pipe_fd[0], &ev))
    {
        close(m_pipe_fd[0]);
        close(m_pipe_fd[1]);
        return false;
    }

    return true;
}

void epoll_reactor::destroy_interrupter()
{
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = m_pipe_fd[0];
    epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, m_pipe_fd[0], &ev);

    close(m_pipe_fd[0]);
    close(m_pipe_fd[1]);
}

//valid op is EPOLL_CTL_ADD, EPOLL_CTL_MOD and EPOLL_CTL_DEL
//return 0 success, other failed
int epoll_reactor::operate(int op, int fd, uint32_t events, epoll_events_handler* events_handler)
{
    struct epoll_event ev;
    ev.events = events | EPOLLET;
    ev.data.ptr = (void*)events_handler;

    return epoll_ctl(m_epoll_fd, op, fd, &ev);
}
