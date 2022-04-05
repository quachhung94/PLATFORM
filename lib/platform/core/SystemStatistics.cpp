/*
Platform - A C++ framework
*/

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#include "spdlog/spdlog.h"

#include "platform/core/SystemStatistics.h"
// #include <fmt/core.h>

static constexpr const char* tag = "MemStat";

namespace platform::core {
TaskStats::TaskStats(uint32_t stack_size) : stack_size(stack_size) { high_water_mark = 0; }

static constexpr const char* dump_fmt = "{:>8} | {:>11} | {:>14} | {:>12} | {:>11} | {:>14} | {:>12}";

void SystemStatistics::dump() const noexcept
{
    { // Only need to lock while accessing the shared data
        synch guard{lock};
        constexpr const char* stack_format = "{:>16} | {:>10} | {:>15} | {:>15}";
        SPDLOG_INFO("");
        SPDLOG_INFO(stack_format, "Name", "Stack", "Min free stack", "Max used stack");

        for (const auto& stat : task_info) {
            SPDLOG_INFO(stack_format, stat.first, stat.second.get_stack_size(), stat.second.get_high_water_mark(),
                         stat.second.get_stack_size() - stat.second.get_high_water_mark());
        }
    }
}
} // namespace platform::core
