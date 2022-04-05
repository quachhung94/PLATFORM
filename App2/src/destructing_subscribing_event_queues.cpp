/*
Platform - A C++ framework
*/

#include "destructing_subscribing_event_queues.h"

using namespace platform::core;

namespace destructing_subscribing_event_queues {
using namespace platform::core::ipc;

App::App() : Application(platform::core::APPLICATION_BASE_PRIO, std::chrono::seconds(1)) {}

void App::init()
{
    sender = std::make_shared<Sender>();
    receiver = std::make_shared<Receiver>();

    sender->start();
    receiver->start();
}

void App::tick() { std::cout << "Tick!" << std::endl; }
} // namespace destructing_subscribing_event_queues
