cmake_minimum_required(VERSION 3.13)
project(LogisticsManagementSystem)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 23)

find_package(GTest REQUIRED)

include_directories(include/)

add_executable(main
    src/main.c
    src/order_array.c
    src/order.c
    src/tools.c
)

enable_testing()

add_executable(OrderTests 
    src/unit_test.cpp
    src/order_array.c
    src/order.c
    src/tools.c
)

add_test(NAME OrderTests COMMAND OrderTests)

target_link_libraries(OrderTests
    GTest::GTest
    GTest::Main
)
