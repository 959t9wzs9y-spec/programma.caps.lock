cmake_minimum_required(VERSION 3.16)
project(CapsLockNotifierBackend LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(caps_monitor STATIC
    src/caps_monitor.cpp
)

target_include_directories(caps_monitor
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(WIN32)
    target_link_libraries(caps_monitor PRIVATE user32 gdi32 advapi32)
endif()

add_executable(caps_monitor_app src/main.cpp)
target_link_libraries(caps_monitor_app PRIVATE caps_monitor)

set_target_properties(caps_monitor_app PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
