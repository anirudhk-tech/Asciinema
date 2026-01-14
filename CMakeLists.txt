cmake_minimum_required(VERSION 3.10)
project(Asciinema
    VERSION 1.0.0
    DESCRIPTION "Real-time video-to-ASCII terminal player"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Dependencies
find_package(OpenCV REQUIRED)
find_package(Curses REQUIRED)
find_package(Threads REQUIRED)

# Include directories
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${OpenCV_INCLUDE_DIRS}
    ${CURSES_INCLUDE_DIRS}
)

# Source files
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Executable
add_executable(asciinema-player ${SOURCES})

target_link_libraries(asciinema-player
    PRIVATE
        ${OpenCV_LIBS}
        ${CURSES_LIBRARIES}
        Threads::Threads
)
