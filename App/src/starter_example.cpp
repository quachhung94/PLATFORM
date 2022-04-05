/*
Platform - A C++ framework
*/

#include "starter_example.h"
#include "platform/core/SystemStatistics.h"
#include "spdlog/spdlog.h"

using namespace platform::core;

namespace starter_example {
// class ATask : public platform::core::Task {
// public:
//     ATask() : platform::core::Task("Other task", 9000, APPLICATION_BASE_PRIO, std::chrono::seconds{1}) {}

//     void tick() override { SPDLOG_INFO("App::Init Hello from other task"); }
// };

// std::shared_ptr<ATask> a_instance;

App::App() : Application(platform::core::APPLICATION_BASE_PRIO, std::chrono::seconds(3)) {}

void App::init()
{
    SPDLOG_INFO("App::Init Starting...");
    // a_instance.start();
    this->a_instance = std::make_shared<ATask>();
    this->a_instance->start();
}

void App::tick()
{
    SPDLOG_INFO("App Hello world!");
    // SystemStatistics::instance().dump();
}
} // namespace starter_example
