#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#include "spdlog/common.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "destructing_subscribing_event_queues.h"

#include "platform/core/SystemStatistics.h"
#include "platform/core/Task.h"
#include "platform/core/ipc/IEventListener.h"
#include "platform/core/ipc/Publisher.h"
#include "platform/core/ipc/SubscribingTaskEventQueue.h"
#include "platform/core/task_priorities.h"
#include "platform/config_constants.h"

#include <fstream>
#include <iostream>
#include <random>

#include <json/json.h>
#include <json/reader.h>

using namespace platform::core;
using namespace platform::core::ipc;

static void spdlog_init()
{   
    std::vector<spdlog::sink_ptr> sinks;
    /* File sink */
    auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_st>("demo2.log", kMaxSize, kMaxFiles);
    file_sink->set_level(spdlog::level::debug);
    file_sink->set_pattern("[%D %H:%M:%S.%e] [%L] %v");

    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::debug);
    console_sink->set_pattern("[%D %H:%M:%S.%e] [%^%L%$] [%s:%#] %v");

    sinks.push_back(file_sink);
    sinks.push_back(console_sink);

    auto logger = std::make_shared<spdlog::logger>(kLogName, begin(sinks), end(sinks));
    logger->flush_on(spdlog::level::debug);

    spdlog::register_logger(logger);
    spdlog::set_default_logger(logger);
}

int main()
{
    spdlog_init();
    SPDLOG_INFO("Hello, {}", kLogName);

    
    platform::core::SystemStatistics::instance().dump();

    auto demo = std::make_shared<destructing_subscribing_event_queues::App>();

    demo->start();

    return 0;
}