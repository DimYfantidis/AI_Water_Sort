cmake_minimum_required(VERSION 3.10)

project(ai_water_sort LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SOURCES
    Bottle.cpp
    MemoryPool.cpp
    main.cpp
)

set(HEADERS
    Bottle.h
    MemoryPool.h
    State.h
    colors.h
    output_util.h
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
