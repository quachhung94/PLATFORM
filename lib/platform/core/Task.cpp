/**
 * @file Task.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2021-11-10
 *
 * @copyright Copyright (c) 2021
 *
 */

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_INFO
#include "spdlog/spdlog.h"

#include "platform/core/SystemStatistics.h"
#include "platform/core/Task.h"
#include "platform/core/ipc/Publisher.h"
#include <algorithm>
#include <utility>

namespace platform::core {
Task::Task(std::string task_name, uint32_t stack_size, uint32_t priority, std::chrono::milliseconds tick_interval,
           int core)
    : name(std::move(task_name)),
      m_worker(),
      m_stackSize(stack_size),
      m_priority(priority),
      m_tickInterval(tick_interval),
      m_isAttached(false),
      affinity(core)
{
}

Task::Task(uint32_t priority, std::chrono::milliseconds tick_interval)
    : name("MainTask"),
      m_stackSize(0),
      m_priority(priority),
      m_tickInterval(tick_interval),
      m_isAttached(true),
      affinity(tskNO_AFFINITY)
{
}

Task::~Task()
{
    SPDLOG_INFO("{}: {}", name, "Task dtor");

    m_loopCondition = false;
    if (m_worker.joinable()) {
        m_worker.join();
    }
    m_notification.clear();
}

void Task::start()
{
    std::unique_lock<std::mutex> lock{m_startMutex};

    if (!m_started) {
        m_statusReportTimer.start();

        if (m_isAttached) {
            SPDLOG_INFO("{}: {}", name, "Running as attached thread");

            // Attaching to another task, just run execute.
            exec();
        }
        else {
            SPDLOG_INFO("{}: {}", name, "Creating worker thread");
            m_worker = std::thread([this]() { this->exec(); });

            SPDLOG_INFO("{}: {}", name, "Waiting for worker to start");

            // To avoid race conditions between tasks during start up,
            // always wait for the new task to start.
            m_startCondition.wait(lock, [this] { return m_started.load(); });

            SPDLOG_INFO("{}: {}", name, "Worker started");
        }
    }
}

void Task::exec()
{
    SPDLOG_INFO("{}: {}", name, "Executing...");

    if (!m_isAttached) {
        SPDLOG_INFO("{}: {}", name, "Notify m_startMutex");
        m_started = true;
        std::unique_lock<std::mutex> lock{m_startMutex};
        m_startCondition.notify_all();
    }

    SPDLOG_INFO("{}: {}", name, "Initializing...");
    init();

    SPDLOG_INFO("{}: {}", name, "Initialized");

    timer::ElapsedTime delayed{};

    delayed.start();

    reportStackStatus();

    while (m_loopCondition) { // for (;;)
        // Try to keep the tick alive even when there are lots of incoming messages
        // by simply not checking the queues when more than one tick interval has passed.
        if (m_tickInterval.count() > 0 && delayed.getRunningTime() > m_tickInterval) {
            tick();
            delayed.reset();
        }
        else {
            std::unique_lock<std::mutex> lock{m_queueMutex};

            // Wait for data to become available, or a timeout to occur.
            auto queue_ptr = m_notification.waitForNotification(m_tickInterval);

            // Check if the weak_ptr we got back is uninitialized using the defined behaviour here:
            // https://en.cppreference.com/w/cpp/memory/weak_ptr/owner_before
            //
            // Quote: The order is such that two smart pointers compare equivalent only
            // if they are both empty or if they both own the same object, even if the
            // values of the pointers obtained by get() are different (e.g. because they
            // point at different subobjects within the same object)
            decltype(queue_ptr) empty_ptr{};

            if (!queue_ptr.owner_before(empty_ptr) && !empty_ptr.owner_before(queue_ptr)) {
                // Timeout - no messages.
                tick();
                delayed.reset();
            }
            else {
                // A queue has signaled an item is available.
                // Note: Do not retrieve all messages from the the queue;
                // it will prevent messages to arrive in the same order
                // they were sent when there are more than one receiver queue.
                auto queue = queue_ptr.lock();

                if (queue) {
                    queue->forwardToEventListener();
                }
            }
        }

        if (m_statusReportTimer.getRunningTime() > std::chrono::seconds(60)) {
            reportStackStatus();
            m_statusReportTimer.reset();
        }
    }
}

void Task::registerQueueWithTask(platform::core::ipc::ITaskEventQueue* task_queue)
{
    task_queue->registerNotification(&m_notification);
}

void Task::reportStackStatus() { SystemStatistics::instance().report(name, TaskStats{m_stackSize}); }
} // namespace platform::core
