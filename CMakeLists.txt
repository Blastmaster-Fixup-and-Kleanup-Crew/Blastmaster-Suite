cmake_minimum_required(VERSION 3.16)

project(BlastmasterSuite VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BLASTMASTER_BUILD_PROFESSIONAL "Build the Professional edition apps." ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Sql)
find_package(SQLite3 REQUIRED)

add_subdirectory(src)

message(STATUS "Blastmaster Suite configured successfully.")
message(STATUS "Professional edition: ${BLASTMASTER_BUILD_PROFESSIONAL}")
