cmake_minimum_required(VERSION 3.7)
project(ctest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions ("-Wall -Wextra -Wunreachable-code")

set(SOURCES
        dungeon.cpp 
        dungeon_generate.cpp
        )

set(HEADERS
        )

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# target_link_libraries(${PROJECT_NAME})