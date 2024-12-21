cmake_minimum_required(VERSION 3.15)
project(OrderSystem)

include_directories(include)

add_executable(main
    src/main.c
    src/menu.c
    src/operation.c
    src/color.c
)

target_link_libraries(main
    OrderLib
    LoggerLib
)