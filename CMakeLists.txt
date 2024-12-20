cmake_minimum_required(VERSION 3.15)
project(LogisticsManagementSystem)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 23)

include_directories(include/)

add_library(OrderLib STATIC
    src/order_array.c
    src/order_struct.c
    src/tools.c
    src/logger.c
)

target_link_directories(OrderLib
    PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

add_subdirectory(app)

add_executable(OrderTests 
    test/main.cpp
    test/unit_test.cpp
)

find_package(GTest REQUIRED)
enable_testing()
add_test(NAME OrderTests COMMAND OrderTests)

target_link_libraries(OrderTests
    OrderLib
    GTest::GTest
    GTest::Main
)
