cmake_minimum_required(VERSION 3.26)
project(LogisticsManagementSystem)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 23)

include_directories(include/)

add_library(LoggerLib STATIC
    src/logger.c
)

add_library(ArrayLib STATIC
    src/array.c
)

target_link_directories(LoggerLib
    PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

target_link_directories(ArrayLib
    PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

add_library(OrderLib STATIC
    src/order_array.c
    src/order_struct.c
    src/tools.c
)

target_link_directories(OrderLib
    PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(OrderLib
    LoggerLib
    ArrayLib
)

add_subdirectory(app)

add_executable(OrderTests 
    test/main.cpp
    test/unit_test.cpp
)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.15.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)

enable_testing()
add_test(NAME OrderTests COMMAND OrderTests)

target_link_libraries(OrderTests
    OrderLib
    LoggerLib
    GTest::gtest
    GTest::gtest_main
)
