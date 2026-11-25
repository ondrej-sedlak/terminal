cmake_minimum_required(VERSION 3.16)
project(terminal CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

# Put the binary in the project root so "./terminal" finds levels/ right away.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

add_executable(terminal
    main.cpp
    NetworkMap.cpp
    InputHandler.cpp
    Renderer.cpp
    GameEngine.cpp
    Menu.cpp
)
target_link_libraries(terminal PRIVATE Threads::Threads)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(terminal PRIVATE -Wall -Wextra)
endif()
