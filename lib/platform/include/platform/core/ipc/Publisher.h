/*
Platform - A C++ framework
*/

#pragma once

#include "Link.h"

namespace platform::core::ipc {
/**
 * @brief Publish item to all subscribers registering for it
 *
 * @tparam T
 */
template <typename T>
class Publisher {
public:
    static void publish(const T& item);
};

template <typename T>
void Publisher<T>::publish(const T& item)
{
    Link<T>::publish(item);
}
} // namespace platform::core::ipc
