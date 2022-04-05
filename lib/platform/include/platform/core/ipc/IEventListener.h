/*
Platform - A C++ framework
*/

#pragma once

namespace platform::core::ipc {
/**
 * @brief Interface for implementation of event listener
 *
 * @tparam EventType
 */
template <typename EventType>
class IEventListener {
public:
    virtual ~IEventListener() = default;

    /// The response method where the event will be received from a TaskEventQueue<EventType>
    /// \param event The event
    virtual void event(const EventType& event) = 0;
};
} // namespace platform::core::ipc
