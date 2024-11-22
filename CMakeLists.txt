# Specify the minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Define the project name and language
project(ai_water_sort LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Specify include directories
include_directories(include)

# Add source files
file(GLOB SOURCES "src/*.cpp")

# Add the executable target
add_executable(${PROJECT_NAME} ${SOURCES})
