cmake_minimum_required(VERSION 3.10)
project(remote_basic_app)

set(CMAKE_CXX_STANDARD 17)

option(NATIVE_BUILD "Build on Host Platform" ON)
# Select the test project to build
set(selected_test_project destructing_subscribing_event_queues)

set(PLATFORM_ENABLE_ASAN 0)
set(PLATFORM_ASAN_OPTIMIZATION_LEVEL 0)

list(APPEND available_tests
        starter_example
        publish
        task_event_queue
        timer
        linux_asan_test
        destructing_subscribing_event_queues
        )

list(FIND available_tests ${selected_test_project} test_found)
if(${test_found} EQUAL -1)
    message(FATAL_ERROR "'${selected_test_project}' not found or not a valid test project for the current platform.")
endif()

message(STATUS "Compiling for Host Platform")

if(NOT CMAKE_BUILD_TYPE)
    # message(STATUS "CMAKE_BUILD_TYPE Release")
    # set(CMAKE_BUILD_TYPE Release)
else()
    message(STATUS "CMAKE_BUILD_TYPE Not Release")
endif()

# add_subdirectory(externals/jsoncpp)

if(NATIVE_BUILD)
set(JSONCPP_LIB_BUILD_SHARED ON CACHE BOOL "Enable jsoncpp shared library" FORCE)
set(JSONCPP_WITH_TESTS OFF CACHE BOOL "Enable jsoncpp tests" FORCE)
set(JSONCPP_WITH_POST_BUILD_UNITTEST OFF CACHE BOOL "Enable jsoncpp post build unit tests" FORCE)
set(JSONCPP_WITH_PKGCONFIG_SUPPORT OFF CACHE BOOL "Enable jsoncpp pkgconfig support" FORCE)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/externals/jsoncpp ${CMAKE_BINARY_DIR}/externals/jsoncpp)
endif()

add_subdirectory(externals/spdlog)
add_subdirectory(App)
add_subdirectory(App2)
add_subdirectory(lib)
# add_subdirectory(utils_apps)
# add_subdirectory(integration_test)

