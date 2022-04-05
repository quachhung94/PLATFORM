/*
Platform - A C++ framework
*/

#pragma once

#include "platform/core/Application.h"
#include "platform/core/task_priorities.h"
#include "platform/core/Task.h"
#include "spdlog/spdlog.h"
#include <iostream>

namespace starter_example {
class ATask : public platform::core::Task {
public:
    ATask() : platform::core::Task("Other task", 9000, platform::core::APPLICATION_BASE_PRIO, std::chrono::seconds{1}) {}

    void tick() override { SPDLOG_INFO("App::Init Hello from other task"); }
};

class App : public platform::core::Application {
public:
    App();

    void init() override;

    void tick() override;

    std::shared_ptr<ATask> a_instance;
};
} // namespace starter_example
