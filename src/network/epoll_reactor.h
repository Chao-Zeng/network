#ifndef _EPOLL_REACTOR_H_
#define _EPOLL_REACTOR_H_

// platform header
#include <pthread.h>

// C C++ header
#include <stdint.h>

/*
valid epoll events

EPOLLIN
The associated file is available for read(2) operations.

EPOLLOUT
The associated file is available for write(2) operations.

EPOLLRDHUP (since Linux 2.6.17)
Stream socket peer closed connection, or shut down writing half of connection.  (This flag is  especially  useful  for  writing
simple code to detect peer shutdown when using Edge Triggered monitoring.)

EPOLLPRI
There is urgent data available for read(2) operations.

EPOLLERR
Error condition happened on the associated file descriptor.  epoll_wait(2) will always wait for this event; it is not necessary
to set it in events.

EPOLLHUP
Hang up happened on the associated file descriptor.  epoll_wait(2) will always wait for this event; it is not necessary to  set
it in events.

EPOLLET
Sets  the  Edge Triggered behavior for the associated file descriptor.  The default behavior for epoll is Level Triggered.  See
epoll(7) for more detailed information about Edge and Level Triggered event distribution architectures.

EPOLLONESHOT (since Linux 2.6.2)
Sets the one-shot behavior for the associated file descriptor.  This means that after an event is pulled out with epoll_wait(2)
the  associated  file  descriptor is internally disabled and no other events will be reported by the epoll interface.  The user
must call epoll_ctl() with EPOLL_CTL_MOD to re-arm the file descriptor with a new event mask.
*/

//forward declaration
class epoll_events_handler;

class epoll_reactor
{
public:
    epoll_reactor(int size = 200000);

    ~epoll_reactor();

    //return 0 success, other failed
    int add(int fd, uint32_t events, epoll_events_handler* events_handler);

    //return 0 success, other failed
    int modify(int fd, uint32_t events, epoll_events_handler* events_handler);

    //return 0 success, other failed
    int remove(int fd, uint32_t events, epoll_events_handler* events_handler);

    void interrupt();

    bool run();

    void stop();

private:
    static void* poll_thread(void* arg);

    bool init_interrupter();

    void destroy_interrupter();

    //valid op is EPOLL_CTL_ADD, EPOLL_CTL_MOD and EPOLL_CTL_DEL
    //return 0 success, other failed
    int operate(int op, int fd, uint32_t events, epoll_events_handler* events_handler);

private:
    // delete copy constructor
    epoll_reactor(const epoll_reactor&);
    // delete copy assignment
    epoll_reactor& operator= (const epoll_reactor&);

private:
    int m_epoll_fd;
    pthread_t m_thread_id;
    volatile bool m_run;
    int m_pipe_fd[2];
    static const int MAX_EVENTS = 200;
};

#endif // _EPOLL_REACTOR_H_
