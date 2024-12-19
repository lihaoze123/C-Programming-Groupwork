cmake_minimum_required(VERSION 3.15)
project(LogisticsManagementSystem)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 23)

find_package(GTest REQUIRED)
enable_testing()
add_test(NAME OrderTests COMMAND OrderTests)

include_directories(include/)

add_library(OrderLib STATIC
    src/order_array.c
    src/order.c
    src/tools.c
    src/logger.c
)

target_link_directories(OrderLib
    PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

add_executable(main
    app/main.c
)

target_link_libraries(main
    OrderLib
)

add_executable(OrderTests 
    test/unit_test.cpp
)

target_link_libraries(OrderTests
    OrderLib
    GTest::GTest
    GTest::Main
)
