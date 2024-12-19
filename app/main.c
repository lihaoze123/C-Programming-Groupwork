#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "order.h"
#include "order_array.h"
#include "tools.h"
#include "logger.h"

Order_array* order_array = NULL;

void save() {
    log_message(LOG_INFO, "保存订单");
    save_orders(order_array, "orders.txt");
    printf("保存成功\n");
}

void print() {
    log_message(LOG_INFO, "打印所有订单");

    if (order_array->size == 0) {
        printf("没有订单\n");
        return;
    }

    printf("\n=== 订单列表 ===\n");
    printf("订单数量: %zu\n", order_array->size);
    printf("------------------------\n");
    print_orders(order_array);
    printf("------------------------\n");
}

void exit_system() {
    log_message(LOG_INFO, "退出系统");
    free_order_array(order_array);
    exit(0);
}

void add() {
    log_message(LOG_INFO, "添加订单");
    printf("请输入订单信息：\n");
    printf("(订单号，寄件人，寄件地址，收件人，收件地址，描述，重量，状态)\n");

    Order order;
    if (!get_input_order(&order)) {
        log_message(LOG_ERROR, "输入订单信息失败");
        return;
    }

    if (!add_order(order_array, &order)) {
        log_message(LOG_ERROR, "添加订单失败");
    }
}

Order* find_order() {
    log_message(LOG_INFO, "查找订单");
    printf("请输入查找方式：\n(1)：精确查找\n(2)：模糊查找\n(0)：返回\n");

    Order* order = NULL;

    int op;
    scanf("%d", &op);

    if (op == 1) {
        printf("请输入查找方式：\n(1)：订单号\n(2)：寄件人\n(3)：收件人\n");

        int op;
        scanf("%d", &op);

        switch (op) {
            case 1:
                printf("请输入订单号：");
                char id[64];
                scanf("%s", id);
                order = get_order_by_id(order_array, id);
                break;
            case 2:
                printf("请输入寄件人：");
                char sender[64];
                scanf("%s", sender);
                order = get_order_by_sender(order_array, sender);
                break;
            case 3:
                printf("请输入收件人：");
                char receiver[64];
                scanf("%s", receiver);
                order = get_order_by_receiver(order_array, receiver);
                break;
            default:
                printf("无效的选项\n");
                break;
        }
    } else if (op == 2) {
        printf("请输入关键词：");
        char keyword[64];
        scanf("%s", keyword);
        order = fuzzy_search_order(order_array, keyword);
    } else if (op == 0) {
        return NULL;
    } else {
        printf("无效的选项\n");
        return NULL;
    }

    if (order == NULL) {
        printf("未找到订单\n");
    }

    puts("\n====================");
    print_order(order);
    puts("\n====================");

    return order;
}

void delete_order(Order* order) {
    log_message(LOG_INFO, "删除订单:%s", order->id);

    remove_order(order_array, order->id);
    printf("删除成功\n");
}

void delete() {
    log_message(LOG_INFO, "删除订单");

    Order* order = find_order();
    if (order == NULL) {
        return;
    }

    delete_order(order);
}

void modify_order(Order* order) {
    log_message(LOG_INFO, "修改订单:%s", order->id);

    printf("请输入要修改的订单信息：\n");
    printf("(1): 寄件人\n");
    printf("(2): 寄件地址\n");
    printf("(3): 收件人\n");
    printf("(4): 收件地址\n");
    printf("(5): 描述\n");
    printf("(6): 重量\n");
    printf("(7): 状态\n");

    int op;
    scanf("%d", &op);

    switch (op) {
        case 1:
            printf("请输入新的寄件人：");
            scanf("%s", order->sender);
            break;
        case 2:
            printf("请输入新的寄件地址：");
            scanf("%s", order->sender_addr);
            break;
        case 3:
            printf("请输入新的收件人：");
            scanf("%s", order->receiver);
            break;
        case 4:
            printf("请输入新的收件地址：");
            scanf("%s", order->receiver_addr);
            break;
        case 5:
            printf("请输入新的描述：");
            scanf("%s", order->description);
            break;
        case 6:
            printf("请输入新的重量：");
            scanf("%lf", &order->weight);
            break;
        case 7:
            printf("请输入新的状态：");
            scanf("%s", order->status);
            break;
        default:
            printf("无效的选项\n");
            break;
    }
}

void modify() {
    log_message(LOG_INFO, "修改订单");

    Order* order = find_order();
    if (order == NULL) {
        return;
    }

    modify_order(order);
}

void query() {
    log_message(LOG_INFO, "查询订单");

    Order* order = find_order();
    if (order == NULL) {
        return;
    }

    printf("请输入要对该订单进行的操作：\n");
    printf("(1): 删除\n");
    printf("(2): 修改\n");
    printf("(0): 返回\n");

    int op;
    scanf("%d", &op);

    switch (op) {
        case 1:
            delete_order(order);
            break;
        case 2:
            modify_order(order);
            break;
        case 0:
            return;
        default:
            printf("无效的选项\n");
            break;
    }
}

typedef void (*Operation)();

typedef struct MenuOption {
    int code;
    const char* description;
    Operation operation;
} MenuOption;


const MenuOption MENU_OPTIONS[] = {
    {0, "退出系统", exit_system},
    {1, "添加订单", add},
    {2, "删除订单", delete},
    {3, "修改订单", modify},
    {4, "查询订单", query},
    {5, "打印所有订单", print},
    {6, "保存订单", save},
};

const size_t MENU_OPTIONS_COUNT = sizeof(MENU_OPTIONS) / sizeof(MENU_OPTIONS[0]);

void print_menu() {
    printf("\n=== 订单管理系统 ===\n");
    for (size_t i = 0; i < MENU_OPTIONS_COUNT; i++) {
        printf("%d. %s\n", MENU_OPTIONS[i].code, MENU_OPTIONS[i].description);
    }
    printf("请选择操作：");
}

int main(int argc, char* argv[]) {
    init_logger("order_system.log");

    order_array = create_order_array(0);
    load_orders(order_array, "orders.txt");

    while (1) {
        print_menu();
        int op;
        scanf("%d", &op);
        log_message(LOG_INFO, "选择操作: %d", op);

        while (getchar() != '\n');

        MENU_OPTIONS[op].operation();
    }

    return 0;
}
