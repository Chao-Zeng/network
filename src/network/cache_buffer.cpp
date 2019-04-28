#include "network/cache_buffer.h"
#include <string.h>
#include <algorithm>
//#include <stdlib.h>

cache_buffer::cache_buffer():m_size(0)
{
    pthread_mutex_init(&m_locker, NULL);
}

cache_buffer::~cache_buffer()
{
    clear();
    pthread_mutex_destroy(&m_locker);
}

bool cache_buffer::push_data(const uint8_t *data, uint32_t len)
{
    st_data_element e;
    e.data = new uint8_t[len];
    if(!e.data)
    {
        return false;
    }
    memcpy(e.data, data, len);
    e.pos = 0;
    e.len = len;
    pthread_mutex_lock(&m_locker);
    m_list.push_back(e);
    pthread_mutex_unlock(&m_locker);
    m_size += len;
    return true;
}

bool cache_buffer::get_data(uint8_t *data, uint32_t len)
{
    pthread_mutex_lock(&m_locker);
    if(m_size < len)
    {
        pthread_mutex_unlock(&m_locker);
        return false;
    }
    uint32_t geted_len = 0;
    std::list<st_data_element>::iterator ite = m_list.begin();
    while(geted_len < len)
    {
        st_data_element& e = *ite;
        uint32_t copy_len = std::min(len - geted_len, e.len);
        memcpy(data + geted_len, e.data + e.pos, copy_len);
        geted_len += copy_len;
        ++ite;
    }
    pthread_mutex_unlock(&m_locker);
    return true;
}

bool cache_buffer::erase_data(uint32_t len)
{
    pthread_mutex_lock(&m_locker);
    if(m_size < len)
    {
        pthread_mutex_unlock(&m_locker);
        return false;
    }
    uint32_t erased_len = 0;
    while(erased_len < len)
    {
        st_data_element& e = m_list.front();
        uint32_t to_erase_len = std::min(len - erased_len, e.len);
        if(to_erase_len == e.len)
        {
            delete[] e.data;
            m_list.pop_front();
        }
        else
        {
            e.pos += to_erase_len;
            e.len -= to_erase_len;
        }
        erased_len += to_erase_len;
    }
    m_size -= len;
    pthread_mutex_unlock(&m_locker);
    return true;
}

void cache_buffer::clear()
{
    pthread_mutex_lock(&m_locker);
    while(!m_list.empty())
    {
        st_data_element& e = m_list.front();
        delete[] e.data;
        m_list.pop_front();
    }
    m_size = 0;
    pthread_mutex_unlock(&m_locker);
}

uint32_t cache_buffer::get_length()
{
    pthread_mutex_lock(&m_locker);
    uint32_t ret = m_size;
    pthread_mutex_unlock(&m_locker);
    return ret;
}
