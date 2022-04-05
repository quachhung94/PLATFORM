/*
Platform - A C++ framework
*/

#pragma once

#include "platform/core/Task.h"
#include "platform/core/ipc/Queue.h"
#include "platform/core/ipc/SubscribingTaskEventQueue.h"
#include "platform/core/ipc/TaskEventQueue.h"
#include <chrono>
#include <unordered_map>
#include <vector>

namespace platform::core {
/**
 * @brief Parent class of Application for init pre-condition before Application
 * 
 */
class EarlyInit : public Task {
public:
    EarlyInit(uint32_t priority, const std::chrono::milliseconds& tick_interval);

    ~EarlyInit() override;
};

/**
 * @brief Main application initialization
 * 
 */
class Application : public EarlyInit {
public:
    /**
     * @brief Construct a new Application object
     * 
     * @param priority 
     * @param tick_interval 
     */
    Application(uint32_t priority, std::chrono::milliseconds tick_interval) : EarlyInit(priority, tick_interval) {}

    void init() override;
};
} // namespace platform::core
