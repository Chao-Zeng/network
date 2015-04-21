#ifndef _CONDITION_QUEUE_H_
#define _CONDITION_QUEUE_H_

//platform header
#include <pthread.h>

//C++ header
#include <cstddef>
#include <deque>

template<typename value_type>
class ConditionQueue
{
public:
    ConditionQueue() : m_queue_max_size(m_queue_default_size)
    {
        pthread_mutex_init(&m_mutex, NULL);
        pthread_cond_init(&m_cond, NULL);

        //m_queue.resize(m_queue_default_size);
    }

    ~ConditionQueue()
    {
        pthread_mutex_destroy(&m_mutex);
        pthread_cond_destroy(&m_cond);

        m_queue.clear();
    }

    bool push(const value_type& element)
    {
        pthread_mutex_lock(&m_mutex);
        if (m_queue.size() > m_queue_max_size)
        {
            pthread_mutex_unlock(&m_mutex);
            return false;
        }

        m_queue.push_front(element);
        pthread_cond_signal(&m_cond);
        pthread_mutex_unlock(&m_mutex);

        return true;
    }

    value_type pop()
    {
        pthread_mutex_lock(&m_mutex);

        while (m_queue.empty())
        {
            pthread_cond_wait(&m_cond, &m_mutex);
        }

        value_type element = m_queue.back();
        m_queue.pop_back();
        pthread_mutex_unlock(&m_mutex);

        return element;
    }

    void set_max_size(size_t max_size)
    {
        m_queue_max_size = max_size;
    }

private:
    //delete copy construction
    ConditionQueue(const ConditionQueue&);

    //delete assign operator
    ConditionQueue operator= (const ConditionQueue&);

private:
    pthread_mutex_t m_mutex;
    pthread_cond_t m_cond;

    std::deque<value_type> m_queue;
    size_t m_queue_max_size;
    static const size_t m_queue_default_size = 100000;
};

#endif // _CONDITION_QUEUE_H_
