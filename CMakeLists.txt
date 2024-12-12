cmake_minimum_required(VERSION 3.13)
project(LogisticsManagementSystem)

set(CMAKE_C_STANDARD 11)

include_directories(include/)

add_executable(main
    src/main.c
    src/order_array.c
    src/tools.c
)
