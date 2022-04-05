function(set_compile_options target)
    # target_compile_options(${target} PUBLIC $<$<COMPILE_LANGUAGE:CXX>: -fno-rtti>)  -Wold-style-cast
    target_compile_options(${target} PRIVATE $<$<COMPILE_LANGUAGE:CXX>: -Wnon-virtual-dtor>)
    target_compile_options(${target} PRIVATE
                                            -Werror 
                                            -Wno-deprecated-declarations
                                            -Wall 
                                            -Wextra
                                            -Wcast-align
                                            -Wunused
                                            -Wconversion
                                            # -Wsign-conversion
                                            -Wlogical-op
                                            -Wdouble-promotion
                                            -Wformat=2
                                            -Wimplicit-fallthrough
                                            -Wcast-align
                                            -Wmisleading-indentation
                                            -Wduplicated-cond
                                            -Wduplicated-branches
                                            -Wnull-dereference)

    if(${PLATFORM_ENABLE_ASAN})
        if(NOT DEFINED PLATFORM_ASAN_OPTIMIZATION_LEVEL)
            message(FATAL_ERROR "PLATFORM_ASAN_OPTIMIZATION_LEVEL not set")
        endif()

        message(STATUS "ASAN enabled, expect performance degradation")
        target_compile_options(${target} PRIVATE -fsanitize=address -fno-omit-frame-pointer -fsanitize-address-use-after-scope -g -O${PLATFORM_ASAN_OPTIMIZATION_LEVEL})
        target_link_libraries(${target} -fsanitize=address)
    endif()
endfunction()
