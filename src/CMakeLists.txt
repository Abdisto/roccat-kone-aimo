cmake_minimum_required(VERSION 3.1.3...3.25 FATAL_ERROR)
project(hidtest C)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    # hidtest is build as a standalone project

    if(POLICY CMP0074)
        # allow using hidapi_ROOT if CMake supports it
        cmake_policy(SET CMP0074 NEW)
    endif()

    find_package(hidapi 0.12 REQUIRED)
    message(STATUS "Using HIDAPI: ${hidapi_VERSION}")
else()
    # hidtest is built as part of the main HIDAPI build
    message(STATUS "Building \"driver\"")
endif()

set(HIDAPI_HIDTEST_TARGETS)
if(NOT WIN32 AND NOT APPLE AND CMAKE_SYSTEM_NAME MATCHES "Linux")
    if(TARGET hidapi::hidraw)
        add_executable(roccat_kone_aimo_hidraw main.c)
        target_link_libraries(roccat_kone_aimo_hidraw hidapi::hidraw)
        list(APPEND HIDAPI_HIDTEST_TARGETS roccat_kone_aimo_hidraw)
    endif()
else()
    add_executable(hidtest main.c)
    target_link_libraries(hidtest hidapi::hidapi)
    list(APPEND HIDAPI_HIDTEST_TARGETS hidtest)
endif()

install(TARGETS ${HIDAPI_HIDTEST_TARGETS}
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
)
