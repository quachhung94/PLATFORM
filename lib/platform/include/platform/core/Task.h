/*
Platform - A C++ framework
*/

#pragma once

#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <map>
#include <mutex>
#include <string>

#include <thread>

#include "platform/core/ipc/ITaskEventQueue.h"
#include "platform/core/ipc/Queue.h"
#include "platform/core/ipc/QueueNotifier.h"
#include "platform/core/timer/ElapsedTime.h"
#include <atomic>

const int tskNO_AFFINITY = std::numeric_limits<int>::max();

namespace platform::core {
/**
 * @brief Object which manages specific task: execution and notification
 *
 */
class Task {
public:
    virtual ~Task();

    void start();

    void registerQueueWithTask(platform::core::ipc::ITaskEventQueue* task_queue);

    Task(const Task&) = delete;

    Task& operator=(const Task&) = delete;

    Task(Task&&) = delete;

    Task& operator=(Task&&) = delete;

protected:
    /**
     * @brief Construct a new Task object from an existing task
     *
     * @param priority Task priority
     * @param tick_interval Tick interval
     */
    Task(uint32_t priority, std::chrono::milliseconds tick_interval);
    /**
     * @brief Construct a new Task object
     *
     * @param task_name Name of task.
     * @param stack_size Size of task
     * @param priority Task priority
     * @param tick_interval Tick interval
     * @param core
     */
    Task(std::string task_name, uint32_t stack_size, uint32_t priority, std::chrono::milliseconds tick_interval,
         int core = tskNO_AFFINITY);
    /**
     * @brief tick() method help to monitor task status
     *
     */
    virtual void tick() {}
    /**
     * @brief Call when task is initialized
     *
     */
    virtual void init() {}

    void reportStackStatus();

    const std::string name;

private:
    void exec();

    std::thread m_worker;
    uint32_t m_stackSize;
    uint32_t m_priority;
    std::chrono::milliseconds m_tickInterval;
    platform::core::ipc::QueueNotifier m_notification{};
    bool m_isAttached;
    int affinity;
    std::atomic_bool m_started{false};
    std::mutex m_startMutex{};
    std::mutex m_queueMutex{};
    std::condition_variable m_startCondition{};
    platform::core::timer::ElapsedTime m_statusReportTimer{};
    bool m_loopCondition{true};
};
} // namespace platform::core
