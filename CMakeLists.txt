cmake_minimum_required(VERSION 3.16)
project(CapsLockNotifier)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compilatore MSVC
if(MSVC)
    add_compile_options(/O2 /W4 /utf-8)
endif()

# Compilatore GCC/MinGW
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    add_compile_options(-O2 -Wall -Wextra)
    if(MINGW)
        add_compile_options(-municode)
    endif()
endif()

add_executable(CapsLockNotifier WIN32 programma.ciulla.cpp)

# Link librerie Windows
if(WIN32)
    target_link_libraries(CapsLockNotifier user32 gdi32 shell32 advapi32)
endif()

# Impostazioni di output
set_target_properties(CapsLockNotifier PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Su MSVC aggiungi opzioni di subsystem
if(MSVC)
    set_target_properties(CapsLockNotifier PROPERTIES
        LINK_FLAGS "/SUBSYSTEM:WINDOWS"
    )
endif()