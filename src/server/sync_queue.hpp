#pragma once

#include <queue>
#include <boost/thread.hpp>

template <typename T>
class SynchronisedQueue
{
private:
    std::queue<T>             m_queue; // Use STL queue to store data
    boost::mutex              m_mutex; // The mutex to synchronise on
    boost::condition_variable m_cond;  // The condition to wait for

public:
    // Add data to the queue and notify others
    void enqueue(const T &data)
    {
        // Acquire lock on the queue
        boost::unique_lock<boost::mutex> lock(m_mutex);

        // Add the data to the queue
        m_queue.push(data);

        // Notify others that data is ready
        m_cond.notify_one();

    } // Lock is automatically released here

    // Get data from the queue. Wait for data if not available
    T dequeue()
    {
        // Acquire lock on the queue
        boost::unique_lock<boost::mutex> lock(m_mutex);

        // When there is no data, wait till someone fills it.
        // Lock is automatically released in the wait and obtained
        // again after the wait
        while (m_queue.size() == 0)
            m_cond.wait(lock);

        // Retrieve the data from the queue
        T result = m_queue.front();
        m_queue.pop();
        return result;

    } // Lock is automatically released here

    size_t count()
    {
        return m_queue.size();
    }
};
