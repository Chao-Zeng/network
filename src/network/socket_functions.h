#ifndef _SOCKET_FUNCTIONS_H_
#define _SOCKET_FUNCTIONS_H_

// C C++ header
#include <string>

std::string get_peer_ip(int sockfd);

short get_peer_port(int sockfd);

std::string get_local_ip(int sockfd);

short get_local_port(int sockfd);

void set_nonblocking(int sockfd);

void set_blocking(int sockfd);

// read all data in tcp cache but less than buff_size. if return equal to buff_size, need read again.
//return vale
// > 0, read bytes
// == 0, peer closed
// < 0, error
int read_all(int sockfd, char* buff, size_t buff_size);

//write all data to tcp cache
//return vale
// > 0, write bytes
// <= 0, error
int write_all(int sockfd, char* buff, size_t buff_size);

// return vale
// < 0, error, others connected socket fd
int connect(const char* ip, short port);

#endif // _SOCKET_FUNCTIONS_H_
