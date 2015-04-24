#include "socket_functions.h"

// platform header
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>

// C C++ header
#include <errno.h>

std::string get_peer_ip(int sockfd)
{
    struct sockaddr_in peer_addr;
    socklen_t len = sizeof(peer_addr);

    if (0 != getpeername(sockfd, (struct sockaddr*)&peer_addr, &len))
    {
        return std::string("");
    }

    return inet_ntoa(peer_addr.sin_addr);
}

short get_peer_port(int sockfd)
{
    struct sockaddr_in peer_addr;
    socklen_t len = sizeof(peer_addr);

    if (0 != getpeername(sockfd, (struct sockaddr*)&peer_addr, &len))
    {
        return -1;
    }

    return ntohs(peer_addr.sin_port);
}

std::string get_local_ip(int sockfd)
{
    struct sockaddr_in local_addr;
    socklen_t len = sizeof(local_addr);

    if (0 != getsockname(sockfd, (struct sockaddr*)&local_addr, &len))
    {
        return std::string("");
    }

    return inet_ntoa(local_addr.sin_addr);
}

short get_local_port(int sockfd)
{
    struct sockaddr_in local_addr;
    socklen_t len = sizeof(local_addr);

    if (0 != getsockname(sockfd, (struct sockaddr*)&local_addr, &len))
    {
        return -1;
    }

    return ntohs(local_addr.sin_port);
}

void set_nonblocking(int sockfd)
{
    fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL, 0) | O_NONBLOCK);
}

void set_blocking(int sockfd)
{
    fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL, 0) & (~O_NONBLOCK));
}

int read_all(int sockfd, char* buff, size_t buff_size)
{
    char* pos = buff;
    size_t bytes_read = 0;

    while (1)
    {
        int ret = read(sockfd, pos, buff_size - bytes_read);

        if (ret < 0 || 0 == ret)
        {
            if (EAGAIN == errno || EWOULDBLOCK == errno)
            {
                break;
            } 
            else
            {
                return ret;
            }
        }

        pos += ret;
        bytes_read += ret;
    }

    return bytes_read;
}

int write_all(int sockfd, char* buff, size_t buff_size)
{
    char* pos = buff;
    size_t bytes_write = 0;

    while (1)
    {
        int ret = write(sockfd, pos, buff_size - bytes_write);

        if (ret < 0 || 0 == ret)
        {
            if (EAGAIN == errno || EWOULDBLOCK == errno)
            {
                break;
            }
            else
            {
                return ret;
            }
        }

        pos += ret;
        bytes_write += ret;

        if (bytes_write == buff_size)
        {
            break;
        }
    }

    return bytes_write;
}

int connect(const char* ip, short port)
{
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0)
    {
        return -1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);

    if (0 != connect(sock_fd, (struct sockaddr*)&addr, sizeof(addr)))
    {
        close(sock_fd);
        return -1;
    }

    return sock_fd;
}
