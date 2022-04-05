/*
Platform - A C++ framework
*/

#include "platform/core/ipc/QueueNotifier.h"
#include <algorithm>
#include <thread>

namespace platform::core::ipc {
void QueueNotifier::notify(const std::weak_ptr<ITaskEventQueue>& queue)
{
    // Lock the queue, emplace the element and trigger condition notify
    std::unique_lock<std::mutex> lock{m_guard};
    m_queues.emplace_back(queue);
    m_cond.notify_one();
}

void QueueNotifier::removeExpiredQueues()
{
    std::unique_lock<std::mutex> lock{m_guard};

    auto new_end = std::remove_if(m_queues.begin(), m_queues.end(), [&](const auto& o) { return o.expired(); });

    m_queues.erase(new_end, m_queues.end());
}

std::weak_ptr<ITaskEventQueue> QueueNotifier::waitForNotification(std::chrono::milliseconds timeout)
{
    std::unique_lock<std::mutex> lock{m_guard};
    std::weak_ptr<ITaskEventQueue> res{};

    if (m_queues.empty()) {
        // Wait until data is available, or timeout.
        auto wait_result = m_cond.wait_until(lock, std::chrono::steady_clock::now() + timeout,
                                             [this]() { return !m_queues.empty(); });

        // At this point we will have the lock again.
        if (wait_result) {
            if (!m_queues.empty()) {
                res = m_queues.front();
                m_queues.pop_front();
            }
        }
    }
    else {
        res = m_queues.front();
        m_queues.pop_front();
    }

    return res;
}
} // namespace platform::core::ipc
