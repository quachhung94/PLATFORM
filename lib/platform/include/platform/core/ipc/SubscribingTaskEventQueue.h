/*
Platform - A C++ framework
*/

#pragma once

#include "ILinkSubscriber.h"
#include "Link.h"
#include "TaskEventQueue.h"
#include "platform/core/util/create_protected.h"

namespace platform::core::ipc {
/**
 * @brief SubscribingTaskEventQueue expand TaskEventQueue<T> functionality by subcribing an event items from a Publisher
 *
 * @tparam T
 */
template <typename T>
class SubscribingTaskEventQueue : public TaskEventQueue<T> {
public:
    /// Destructor
    ~SubscribingTaskEventQueue() { link.unsubscribe(&wrapper); }

    SubscribingTaskEventQueue() = delete;

    SubscribingTaskEventQueue(const SubscribingTaskEventQueue&) = delete;

    SubscribingTaskEventQueue(SubscribingTaskEventQueue&&) = delete;

    SubscribingTaskEventQueue& operator=(const SubscribingTaskEventQueue&) = delete;

    SubscribingTaskEventQueue& operator=(const SubscribingTaskEventQueue&&) = delete;

    bool push(const T& item) override
    {
        return this->pushInternal(item, this->template shared_from_base<SubscribingTaskEventQueue<T>>());
    }

    static auto create(int size, Task& task, IEventListener<T>& listener)
    {
        auto queue = platform::core::util::create_protected_shared<SubscribingTaskEventQueue<T>>(size, task, listener);
        queue->link_up();

        return queue;
    }

protected:
    /**
     * @brief Construct a new Subscribing Task Event Queue object
     *
     * @param size The size of the queue.
     * @param task The task listen the signal from available event
     * @param listener The receiver of the event (any object inherit IEventListener class)
     */
    SubscribingTaskEventQueue(int size, Task& task, IEventListener<T>& listener)
        : TaskEventQueue<T>(size, task, listener), link()
    {
    }

private:
    void link_up()
    {
        wrapper = LinkWrapper{this->template shared_from_base<SubscribingTaskEventQueue<T>>()};
        link.subscribe(&wrapper);
    }

    class LinkWrapper : public ILinkSubscriber<T> {
    public:
        explicit LinkWrapper(std::weak_ptr<SubscribingTaskEventQueue<T>> queue) : queue(std::move(queue)) {}

        LinkWrapper() = default;

        LinkWrapper(const LinkWrapper&) = delete;

        LinkWrapper(LinkWrapper&&) = delete;

        LinkWrapper& operator=(const LinkWrapper&) = delete;

        LinkWrapper& operator=(LinkWrapper&&) noexcept = default;

        ~LinkWrapper() = default;

        bool receivePublishedData(const T& data) override
        {
            bool res = true;
            auto q   = queue.lock();

            if (q) {
                res = q->push(data);
            }

            return res;
        }

    private:
        std::weak_ptr<SubscribingTaskEventQueue<T>> queue;
    };

    Link<T> link;
    LinkWrapper wrapper;
};
} // namespace platform::core::ipc
