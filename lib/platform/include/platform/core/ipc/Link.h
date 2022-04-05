/*
Platform - A C++ framework
*/

#pragma once

#include "ILinkSubscriber.h"
#include "Queue.h"
#include <chrono>
#include <forward_list>
#include <mutex>

namespace platform::core::ipc {
/**
 * @brief Class help link subcribers to a specific event item type
 *
 * @tparam T
 */
template <typename T>
class Link {
public:
    Link() = default;

    Link(const Link&) = delete;

    /**
     * @brief Destroy the Link object, being implement in child class
     *
     */
    virtual ~Link() = default;

    /**
     * @brief Subcribe to message
     *
     * @param subscriber
     */
    void subscribe(ILinkSubscriber<T>* subscriber);

    /**
     * @brief Unsubcribe to message
     *
     * @param subscriber
     */
    void unsubscribe(ILinkSubscriber<T>* subscriber);

    /**
     * @brief Publish item to all subcribers
     *
     * @param item Event items
     * @return true if all subcribers can successfully receive the item
     * @return false
     */
    static bool publish(const T& item)
    {
        std::lock_guard<std::mutex> l(get_mutex());
        bool res = false;

        for (auto subscriber : get_subscribers()) {
            res &= subscriber->receivePublishedData(item);
        }

        return res;
    }

private:
    static std::forward_list<ILinkSubscriber<T>*>& get_subscribers();

    static std::mutex& get_mutex()
    {
        static std::mutex m;

        return m;
    }
};

template <typename T>
void Link<T>::subscribe(ILinkSubscriber<T>* subscriber)
{
    std::lock_guard<std::mutex> l(get_mutex());
    get_subscribers().push_front(subscriber);
}

template <typename T>
void Link<T>::unsubscribe(ILinkSubscriber<T>* subscriber)
{
    std::lock_guard<std::mutex> l(get_mutex());
    get_subscribers().remove(subscriber);
}

template <typename T>
std::forward_list<ILinkSubscriber<T>*>& Link<T>::get_subscribers()
{
    static std::forward_list<ILinkSubscriber<T>*> subscribers;

    return subscribers;
}
} // namespace platform::core::ipc
