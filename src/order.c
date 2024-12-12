#include "order.h"

void create_order(Order* order, const char* id, const char* sender, const char* sender_addr, const char* receiver, const char* receiver_addr, const char* description, double weight, const char* status);

int print_order(Order* order) {
    if (order == NULL)
        return 0;

    printf("运单号: %s, 寄件人: %s, 寄件地址: %s, 收件人: %s, 收件地址: %s, 物品描述: %s, 重量: %lf, 当前状态: %s\n", 
           order->id,
           order->sender,
           order->sender_addr,
           order->receiver,
           order->receiver_addr,
           order->description,
           order->weight,
           order->status
           );

    return 1;
}

#define DEF_UPDATE_ORDER_STR(__PROPERTY) \
void update_order_##__PROPERTY(Order* order, const char* __PROPERTY) { \
    strcpy(order->__PROPERTY, __PROPERTY); \
}

DEF_UPDATE_ORDER_STR(id)
DEF_UPDATE_ORDER_STR(sender)
DEF_UPDATE_ORDER_STR(sender_addr)
DEF_UPDATE_ORDER_STR(receiver)
DEF_UPDATE_ORDER_STR(receiver_addr)
DEF_UPDATE_ORDER_STR(description)
DEF_UPDATE_ORDER_STR(status)
