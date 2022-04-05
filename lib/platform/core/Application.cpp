/*
Platform - A C++ framework
*/

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic pop

#include "platform/core/Application.h"

using namespace std::chrono;

namespace platform::core {
EarlyInit::EarlyInit(uint32_t priority, const std::chrono::milliseconds& tick_interval) : Task(priority, tick_interval)
{
}

EarlyInit::~EarlyInit() {}

void Application::init() {}
} // namespace platform::core
