/*
Platform - A C++ framework
*/

#pragma once

#include <memory>
#include <type_traits>

namespace platform::core::util {
template <typename T, typename... Args>
auto create_protected_shared(Args&&... args)
{
    class CreationWrapper : public T {
    public:
        explicit CreationWrapper(Args&&... args) : T(std::forward<Args>(args)...) {}
    };

    return std::make_shared<CreationWrapper>(std::forward<Args>(args)...);
}

template <typename T, typename... Args>
auto create_protected_unique(Args&&... args)
{
    class CreationWrapper : public T {
    public:
        explicit CreationWrapper(Args&&... args) : T(std::forward<Args>(args)...) {}
    };

    return std::make_unique<CreationWrapper>(std::forward<Args>(args)...);
}
} // namespace platform::core::util
