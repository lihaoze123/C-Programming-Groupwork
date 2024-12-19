#include <gtest/gtest.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
extern "C" {
    #include "order.h"
    #include "order_array.h"
    #include "tools.h"
    #include "logger.h"
}

// 测试环境设置
class OrderTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 初始化测试数据
        order_array = create_order_array(0);
        order = (Order*) malloc(sizeof(Order));
        init_logger(NULL);
    }

    void TearDown() override {
        // 清理测试数据
        free(order);
        free_order_array(order_array);
    }

    Order_array* order_array;
    Order* order;
};

// 测试打印所有订单
TEST_F(OrderTest, PrintOrders) {
    create_order(order, "123", "Sender", "Sender Address", "Receiver", "Receiver Address", "Description", 5.0, "Pending");
    
    add_order(order_array, order);

    testing::internal::CaptureStdout();
    print_orders(order_array);
    
    std::string output = testing::internal::GetCapturedStdout();
    
    EXPECT_TRUE(output.find("123") != std::string::npos);
}

// 测试加载订单
TEST_F(OrderTest, LoadOrders) {
    FILE* fp = fopen("test_orders.txt", "w");
    
    fprintf(fp , "%s %s %s %s %s %s %lf %s\n", 
            "124",
            "Sender2",
            "Address2",
            "Receiver2",
            "Address2",
            "Description2",
            10.0,
            "Pending");
    
    fclose(fp);

    ASSERT_TRUE(load_orders(order_array, "test_orders.txt"));
    std::cerr << 1 << '\n';
    
    EXPECT_EQ(order_array->size, 1);

    Order* loaded_order = at(order_array, 0);
    
    EXPECT_STREQ(loaded_order->id, "124");

    // 清理文件
    remove("test_orders.txt");
}

// 测试保存订单
TEST_F(OrderTest, SaveOrders) {
    create_order(order, "125", "Sender3", "Address3", 
                 "Receiver3", 
                 "Address3",
                 "Description3",
                 15.0,
                 "Pending");

    add_order(order_array, order);

    ASSERT_TRUE(save_orders(order_array, "saved_orders.txt"));

    // 验证文件内容
    FILE* fp = fopen("saved_orders.txt", "r");
    
    char buffer[512];
    
    fgets(buffer, sizeof(buffer), fp);

    fclose(fp);

    EXPECT_TRUE(strstr(buffer, order->id) != nullptr);

   // 清理文件
   remove("saved_orders.txt");
}

// 测试删除特定订单
TEST_F(OrderTest, RemoveOrder) {
   create_order(order, "126", 
                "Sender4",
                "", 
                "", 
                "", 
                "", 
                20.0,
                "");

   add_order(order_array, order);

   ASSERT_TRUE(remove_order(order_array,"126"));
   EXPECT_EQ(order_array->size , 0); // 数组大小应为零

   ASSERT_FALSE(remove_order(order_array,"126")); // 再次尝试删除应失败
}

// 测试排序功能
TEST_F(OrderTest, IsSortedOrders) {
   create_order(order,"128","Sender6","Address6","Receiver6","Address6","Description6" ,30.0,"Pending");
   add_order(order_array ,order);

   create_order(order,"127","Sender5","Address5","Receiver5","Address5","Description5",25.0,"Pending");
   add_order(order_array ,order);

   EXPECT_FALSE(is_sorted(order_array->orders, order_array->size, sizeof(Order), comp_by_id));

   std::swap(order_array->orders[0], order_array->orders[1]);
   EXPECT_TRUE(is_sorted(order_array->orders, order_array->size, sizeof(Order), comp_by_id));
}

TEST_F(OrderTest, SortOrders) {
   create_order(order,"128","Sender6","Address6","Receiver6","Address6","Description6" ,30.0,"Pending");
   add_order(order_array ,order);

   create_order(order,"127","Sender5","Address5","Receiver5","Address5","Description5",25.0,"Pending");
   add_order(order_array ,order);

   sort_orders (order_array ,comp_by_id );
   EXPECT_STREQ(at (order_array ,0)->id ,"127"); // 检查排序是否正确
}

TEST_F(OrderTest, EditDistance) {
    EXPECT_EQ(edit_distance("hello", "hello"), 0);
    EXPECT_EQ(edit_distance("hello", "helo"), 1);
    EXPECT_EQ(edit_distance("kitten", "sitting"), 3);
    EXPECT_EQ(edit_distance("", "abc"), 3);
    EXPECT_EQ(edit_distance("abc", ""), 3);
    EXPECT_EQ(edit_distance("", ""), 0);
}

TEST_F(OrderTest, Logger) {
    log_message(LOG_DEBUG, "This is a test message");
    log_message(LOG_INFO, "This is a test message");
    log_message(LOG_WARNING, "This is a test message");
    log_message(LOG_ERROR, "This is a test message");
}

TEST_F(OrderTest, LoggerFile) {
    FILE* fp = init_logger("test.log");
    log_message(LOG_DEBUG, "This is a test message");
    log_message(LOG_INFO, "This is a test message");
    log_message(LOG_WARNING, "This is a test message");
    log_message(LOG_ERROR, "This is a test message");
    close_logger();

    fp = fopen("test.log", "r");
    char buffer[512];
    fgets(buffer, sizeof(buffer), fp);
    fclose(fp);

    EXPECT_TRUE(strstr(buffer, "This is a test message") != nullptr);

    remove("test.log");
}

TEST_F(OrderTest, GetOrderByStr) {
    create_order(order, "129", "Sender7", "Address7", "Receiver7", "Address7", "Description7", 35.0, "Pending");
    add_order(order_array, order);

    char buffer[512];

    Order* find = get_order_by_id(order_array, "129");
    sprint_order(find, buffer);
    log_message(LOG_DEBUG, buffer);
    EXPECT_EQ(strcmp(find->id, order->id) == 0, true);

    find = get_order_by_sender(order_array, "Sender7");
    sprint_order(find, buffer);
    log_message(LOG_DEBUG, buffer);
    EXPECT_EQ(strcmp(find->sender, order->sender) == 0, true);

    find = get_order_by_receiver(order_array, "Receiver7");
    sprint_order(find, buffer);
    log_message(LOG_DEBUG, buffer);
    EXPECT_EQ(strcmp(find->receiver, order->receiver) == 0, true);
}

TEST_F(OrderTest, FuzzySearchOrder) {
    create_order(order, "130", "Sender8", "Address8", "Receiver8", "Address8", "Description8", 40.0, "Pending");
    add_order(order_array, order);

    create_order(order, "131", "Sender9", "Address9", "Receiver9", "Address9", "Description9", 45.0, "Pending");
    add_order(order_array, order);

    Order* find = fuzzy_search_order(order_array, "Sender8");
    EXPECT_EQ(strcmp(find->id, at(order_array, 0)->id) == 0, true);

    find = fuzzy_search_order(order_array, "Desc8");
    EXPECT_EQ(strcmp(find->id, at(order_array, 0)->id) == 0, true);

    find = fuzzy_search_order(order_array, "9");
    EXPECT_EQ(strcmp(find->id, at(order_array, 1)->id) == 0, true);
}

int main(int argc , char **argv) {
    if (1) {
        ::testing::InitGoogleTest(&argc ,argv);
        return RUN_ALL_TESTS();
    } else {
        Order order;
        create_order(&order,"127","Sender5","Address5","Receiver5","Address5","Description5",25.0,"Pending");

        print_order(&order);

        auto array = create_order_array(1);
        array->orders[0] = order;

        save_orders(array, "saved_orders.txt");
    } 
}
