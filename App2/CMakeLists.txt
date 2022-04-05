# Link libraries
# list(APPEND LINKLIBS ${OPENSSL_LIBRARIES})
list(APPEND LINKLIBS spdlog)
list(APPEND LINKLIBS platform)
list(APPEND LINKLIBS jsoncpp)

# OpenSSL libraries
# message(STATUS "OpenSSL version: ${OPENSSL_VERSION} ${OPENSSL_INCLUDE_DIR} ${OPENSSL_LIBRARIES}")

add_library(jsoncpp SHARED IMPORTED) # or STATIC instead of SHARED
    set_target_properties(jsoncpp PROPERTIES
    IMPORTED_LOCATION "${CMAKE_BINARY_DIR}/lib/libjsoncpp.so.1.9.5"
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/../externals/jsoncpp/include"
    )
    
# System directories
include_directories(SYSTEM "${CMAKE_CURRENT_LIST_DIR}/include" ${INCLUDE_DIRECTORIES})

# client
file(GLOB EXAMPLE_HEADER_FILES "include/*.h")
# file(GLOB EXAMPLE_INLINE_FILES "*.inl")
file(GLOB EXAMPLE_SOURCE_FILES "src/*.cpp")

add_executable(demo2 ${EXAMPLE_SOURCE_FILES})
target_link_libraries(demo2 ${LINKLIBS})
