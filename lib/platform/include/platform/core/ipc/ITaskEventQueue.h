/*
Platform - A C++ framework
*/

#pragma once

namespace platform::core::ipc {

class QueueNotifier;
/**
 * @brief Interface for TaskEventQueue
 *
 */
class ITaskEventQueue {
public:
    ITaskEventQueue() = default;

    virtual ~ITaskEventQueue() = default;

    /**
     * @brief Forwared event to listener
     *
     */
    virtual void forwardToEventListener() = 0;

    /**
     * @brief Return the size of queue
     *
     * @return Size of queue
     */
    virtual int size() = 0;
    /**
     * @brief Register a QueueNotifier, attach to the TaskEventQueue
     *
     * @param notification Pointer to QueueNotifier
     */
    virtual void registerNotification(QueueNotifier* notification) = 0;
};
} // namespace platform::core::ipc
