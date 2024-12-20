#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <gtest/gtest.h>

int main(int argc , char **argv) {
    if (1) {
        ::testing::InitGoogleTest(&argc ,argv);
        return RUN_ALL_TESTS();
    } else {
        // Do something else
    } 
}