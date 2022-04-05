/*
Platform - A C++ framework
*/

#pragma once

namespace platform::core::ipc {
/**
 * @brief Interface for link subcriber
 *
 * @tparam T
 */
template <typename T>
class ILinkSubscriber {
public:
    virtual ~ILinkSubscriber() = default;

    virtual bool receivePublishedData(const T& data) = 0;
};
} // namespace platform::core::ipc
