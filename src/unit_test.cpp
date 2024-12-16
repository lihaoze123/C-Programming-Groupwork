#include <gtest/gtest.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
extern "C" {
    #include "order.h"
    #include "order_array.h"
    #include "tools.h"
}

// 测试环境设置
class OrderTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 初始化测试数据
        order_array = create_order_array(0);
        order = (Order*) malloc(sizeof(Order));
    }

    void TearDown() override {
        // 清理测试数据
        free(order);
        free_order_array(order_array);
    }

    Order_array* order_array;
    Order* order;
};

// 测试创建订单
TEST_F(OrderTest, CreateOrder) {
    create_order(order, "123", "Sender", "Sender Address", "Receiver", "Receiver Address", "Description", 5.0, "Pending");

    EXPECT_STREQ(order->id, "123");
    EXPECT_STREQ(order->sender, "Sender");
    EXPECT_STREQ(order->sender_addr, "Sender Address");
    EXPECT_STREQ(order->receiver, "Receiver");
    EXPECT_STREQ(order->receiver_addr, "Receiver Address");
    EXPECT_STREQ(order->description, "Description");
    EXPECT_DOUBLE_EQ(order->weight, 5.0);
    EXPECT_STREQ(order->status, "Pending");
}

// 测试打印订单
TEST_F(OrderTest, PrintOrder) {
    create_order(order, "123", "Sender", "Sender Address", "Receiver", "Receiver Address", "Description", 5.0, "Pending");

    testing::internal::CaptureStdout();
    print_order(order);
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_TRUE(output.find("123") != std::string::npos);
}

// 测试计算价格
TEST_F(OrderTest, CalcPrice) {
    create_order(order, "123", "Sender", "Sender Address", "Receiver", "Receiver Address", "Description", 5.0, "Pending");

    double price = calc_price(order);
    EXPECT_DOUBLE_EQ(price, 50.0); // 10.0 * weight
}

// 测试创建订单数组
TEST_F(OrderTest, CreateOrderArray) {
    Order_array* array = create_order_array(5);
    
    ASSERT_NE(array, nullptr);
    EXPECT_EQ(array->size, 5);

    free_order_array(array);
}

// 测试添加订单
TEST_F(OrderTest, AddOrder) {
    create_order(order, "123", "Sender", "Sender Address", "Receiver", "Receiver Address", "Description", 5.0, "Pending");
    
    ASSERT_TRUE(add_order(order_array, order));
    EXPECT_EQ(order_array->size, 1);
}

// 测试获取订单
TEST_F(OrderTest, GetOrder) {
    create_order(order, "123", "Sender", "Sender Address", "Receiver", "Receiver Address", "Description", 5.0, "Pending");
    
    add_order(order_array, order);
    
    Order* retrieved_order = at(order_array, 0);
    
    ASSERT_NE(retrieved_order, nullptr);
    EXPECT_STREQ(retrieved_order->id, order->id);
}

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