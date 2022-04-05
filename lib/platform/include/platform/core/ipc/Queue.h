/*
Platform - A C++ framework
*/

#pragma once

#include <algorithm>
#include <chrono>
#include <mutex>
#include <string>
#include <vector>
namespace platform::core::ipc {
/**
 * @brief A queue that hold items of type T. Thread-safe design
 *
 * @tparam T
 */
template <typename T>
class Queue {
public:
    /**
     * @brief Construct a new Queue object
     *
     * @param size Size of the queue
     */
    explicit Queue(int size) : queue_size(size), items(), m_guard() { items.reserve(static_cast<size_t>(size)); }

    /// Destructor
    virtual ~Queue()
    {
        std::lock_guard<std::mutex> lock(m_guard);
        items.clear();
    }

    /**
     * @brief Get size of the queue
     *
     * @return int
     */
    int size()
    {
        std::lock_guard<std::mutex> lock(m_guard);

        return queue_size;
    }

    /**
     * @brief Push item into the queue
     *
     * @param item
     * @return true if success to push into the queue
     * @return false
     */
    bool push(const T& item)
    {
        std::lock_guard<std::mutex> lock(m_guard);

        bool res = items.size() < static_cast<size_t>(queue_size);

        if (res) {
            items.emplace_back(item);
        }

        return res;
    }

    /**
     * @brief Pop an item off the queue
     *
     * @param target Reference to <T> target to store item receving from queue
     * @return true if success to receive the item
     * @return false
     */
    bool pop(T& target)
    {
        std::lock_guard<std::mutex> lock(m_guard);

        bool res = items.size() > 0;

        if (res) {
            target = items.front();
            items.erase(items.begin());
        }

        return res;
    }
    /**
     * @brief Get item from the queue, not pop
     *
     * @param target Reference to <T> target to store item receving from queue
     * @return true if success to receive the item
     * @return false
     */
    bool front(T& target)
    {
        std::lock_guard<std::mutex> lock(m_guard);

        bool res = items.size() > 0;

        if (res) {
            target = items.front();
        }

        return res;
    }

    /**
     * @brief Return if the queue is empty or not
     *
     * @return true if empty
     * @return false if not empty
     */
    bool empty() { return count() == 0; }

    /**
     * @brief Return number of item in the queue
     *
     * @return int
     */
    int count()
    {
        std::lock_guard<std::mutex> lock(m_guard);

        return static_cast<int>(items.size());
    }

private:
    const int queue_size;
    std::vector<T> items;
    std::mutex m_guard;
};
} // namespace platform::core::ipc
