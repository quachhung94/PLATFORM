/*
Platform - A C++ framework
*/

#pragma once

#include "platform/core/Application.h"
#include "platform/core/task_priorities.h"
#include "platform/core/ipc/IEventListener.h"
#include "platform/core/ipc/Publisher.h"
#include "platform/core/ipc/SubscribingTaskEventQueue.h"
#include "spdlog/spdlog.h"
#include <iostream>
#include <random>

namespace destructing_subscribing_event_queues {

class Item {
};

class Sender : public platform::core::Task {
public:
    Sender() : platform::core::Task("Sender", 9000, platform::core::APPLICATION_BASE_PRIO, std::chrono::milliseconds{10}) {}

    void tick() override { platform::core::ipc::Publisher<Item>::publish(Item{}); }
};

class Receiver : public platform::core::Task, public platform::core::ipc::IEventListener<Item> {
public:
    using ReveiverQueue_t = platform::core::ipc::SubscribingTaskEventQueue<Item>;

    Receiver()
        : platform::core::Task("Receiver", 9000, platform::core::APPLICATION_BASE_PRIO, std::chrono::milliseconds{500}),
          rd(),
          gen(rd()),
          dis(0, 1)
    {
    }

    void tick() override
    {
        auto val    = dis(gen);
        auto adding = val == 1;

        if (adding) {
            for (int i = 0; i < 10; ++i) {
                queues.emplace_back(ReveiverQueue_t::create(50, *this, *this));
            }
        }
        else {
            for (int i = 0; i < 10 - val && !queues.empty(); ++i) {
                queues.erase(queues.begin());
                removed++;
            }
        }
    }

    void event(const Item& /*value*/) override
    {
        static size_t last_count  = 0;
        static size_t event_count = 0;
        event_count++;

        if (last_count != queues.size()) {
            last_count = queues.size();
            SPDLOG_INFO("Rec Queue count: {}, Removed: {}, Evt count: {}", last_count, removed, event_count);
        }
    }

private:
    std::vector<std::shared_ptr<ReveiverQueue_t>> queues{};
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_int_distribution<> dis;
    std::size_t removed = 0;
};

class App : public platform::core::Application {
public:
    App();

    void init() override;

    void tick() override;

    std::shared_ptr<Sender> sender;
    std::shared_ptr<Receiver> receiver;
};
} // namespace destructing_subscribing_event_queues
