/*
Platform - A C++ framework
*/

#pragma once

#include "IEventListener.h"
#include "ITaskEventQueue.h"
#include "QueueNotifier.h"
#include "platform/core/Task.h"
#include "platform/core/util/create_protected.h"
#include <memory>

namespace platform::core::ipc {
/**
 * @brief TaskEventQueue expand the Queue<T> functionality by attaching it to a Task and signal event items to
 * registered listeners
 *
 * @tparam T
 */
template <typename T>
class TaskEventQueue : public ITaskEventQueue, public std::enable_shared_from_this<TaskEventQueue<T>> {
public:
    friend core::Task;

    static_assert(std::is_default_constructible<T>::value, "DataType must be default-constructible");
    static_assert(std::is_assignable<T, T>::value, "DataType must be a assignable");

    static auto create(int size, Task& owner_task, IEventListener<T>& event_listener)
    {
        return platform::core::util::create_protected_shared<TaskEventQueue<T>>(size, owner_task, event_listener);
    }

    ~TaskEventQueue() override { notif->removeExpiredQueues(); }

    TaskEventQueue() = delete;

    TaskEventQueue(const TaskEventQueue&) = delete;

    TaskEventQueue(TaskEventQueue&&) = delete;

    TaskEventQueue& operator=(const TaskEventQueue&) = delete;

    TaskEventQueue& operator=(const TaskEventQueue&&) = delete;
    /**
     * @brief Push an item into the queue
     *
     * @param item
     * @return true if the queue could accept the item
     * @return false
     */
    virtual bool push(const T& item) { return pushInternal(item, this->shared_from_this()); }

    // Method size() of ITaskEventQueue
    int size() override { return queue.size(); }
    /**
     * @brief Get number of items currently in queue
     *
     * @return Number of items currently in queue
     */
    int count() { return queue.count(); }

    // Method size() of ITaskEventQueue
    void registerNotification(QueueNotifier* notification) override { notif = notification; }

    void clear()
    {
        while (!queue.empty()) {
            T t;
            queue.pop(t);
        }
    }

protected:
    /**
     * @brief Construct a new Task Event Queue object
     *
     * @param size The size of the queue
     * @param task The task listen the signal from available event
     * @param listener The receiver of the event (any object inherit IEventListener class)
     */
    TaskEventQueue(int size, Task& task, IEventListener<T>& listener) : queue(size), task(task), listener(listener)
    {
        task.registerQueueWithTask(this);
    }

    bool pushInternal(const T& item, const std::weak_ptr<ITaskEventQueue>& receiver)
    {
        auto res = queue.push(item);

        if (res) {
            notif->notify(receiver);
        }

        return res;
    }

    template <typename Derived>
    std::shared_ptr<Derived> shared_from_base()
    {
        return std::static_pointer_cast<Derived>(this->shared_from_this());
    }

    Queue<T> queue;
    QueueNotifier* notif = nullptr;

private:
    void forwardToEventListener() override
    {
        T m;

        if (queue.pop(m)) {
            listener.event(m);
        }
    }

    Task& task;
    IEventListener<T>& listener;
};
} // namespace platform::core::ipc
