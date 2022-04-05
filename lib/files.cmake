set(PLATFORM_LIB_ROOT ${CMAKE_CURRENT_LIST_DIR})

set(platform_dir ${CMAKE_CURRENT_LIST_DIR}/platform)
set(platform_inc_dir ${CMAKE_CURRENT_LIST_DIR}/platform/include/platform)

set(PLATFORM_SOURCES
        ${platform_dir}/core/Application.cpp
        ${platform_dir}/core/ipc/QueueNotifier.cpp
        # ${platform_dir}/core/logging/log.cpp
        ${platform_dir}/core/SystemStatistics.cpp
        ${platform_dir}/core/Task.cpp
        ${platform_dir}/core/timer/ElapsedTime.cpp
        ${platform_dir}/core/timer/Timer.cpp
        ${platform_dir}/core/timer/TimerService.cpp
        )



