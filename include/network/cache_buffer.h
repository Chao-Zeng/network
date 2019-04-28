#ifndef _CACHE_BUFFER_H_
#define _CACHE_BUFFER_H_

#include <pthread.h>
#include <stdint.h>
#include <list>

struct st_data_element
{
    uint8_t *data;
    uint32_t pos;
    uint32_t len;
};

class cache_buffer
{
public:
    cache_buffer();
    ~cache_buffer();
    bool push_data(const uint8_t *data, uint32_t len);
    bool get_data(uint8_t *data, uint32_t len);
    bool erase_data(uint32_t len);
    uint32_t get_length();
    void clear();

private:
    pthread_mutex_t m_locker;
    std::list<st_data_element> m_list;
    uint32_t m_size;
};

#endif // _CACHE_BUFFER_H_
