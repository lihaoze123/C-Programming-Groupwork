#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <gtest/gtest.h>

extern "C" {
    #include "order.h"
    #include "order_array.h"
    #include "tools.h"
    #include "logger.h"
}

int main(int argc , char **argv) {
    if (1) {
        ::testing::InitGoogleTest(&argc ,argv);
        return RUN_ALL_TESTS();
    } else {
        // Do something else
    } 
}