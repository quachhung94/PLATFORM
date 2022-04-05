/*
Platform - A C++ framework
*/

#pragma once

#include "ITaskEventQueue.h"
#include <condition_variable>
#include <deque>
#include <memory>
#include <mutex>

namespace platform::core::ipc {
class QueueNotifier {
public:
    QueueNotifier() = default;

    ~QueueNotifier() = default;
    /**
     * @brief After pushing an item into the queue, trigger an notification to recevier. This then, will be unlock in
     * the loop by waitForNotification() method
     *
     * @param queue
     */
    void notify(const std::weak_ptr<ITaskEventQueue>& queue);

    void removeExpiredQueues();
    /**
     * @brief Call in the loop, wait until an unlock signal be triggered:
     * 1. A new data available in the queue or
     * 2. Timeout arrived
     * @param timeout
     * @return std::weak_ptr<ITaskEventQueue>
     */

    std::weak_ptr<ITaskEventQueue> waitForNotification(std::chrono::milliseconds timeout);

    void clear()
    {
        std::lock_guard<std::mutex> lock(m_guard);
        m_queues.clear();
    }

private:
    std::deque<std::weak_ptr<ITaskEventQueue>> m_queues{};
    std::mutex m_guard{};
    std::condition_variable m_cond{};
};
} // namespace platform::core::ipc
