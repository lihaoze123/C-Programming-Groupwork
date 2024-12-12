#include "tools.h"

void greeting() {
    printf("欢迎使用物流快递运单管理系统！\n");
    printf("您可以执行以下操作：\n");
    printf("1. 添加新运单\n");
    printf("2. 查看所有运单\n");
    printf("3. 查找运单\n");
    printf("4. 删除运单\n");
    printf("5. 更新运单状态\n");
    printf("请输入您的选择 (1-5): ");
}

void test() {
    // 测试 Order_array 部分
    Order_array* a = create_order_array(0);
    Order* order = (Order*) malloc(sizeof(Order));
    add_order(a, order);
    add_order(a, order);
    update_order_id(at(a, 0), "123");
    update_order_id(at(a, 1), "456");
    remove_order(a, "456");
    print_orders(a);
    free_order_array(a);
}
