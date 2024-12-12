#ifndef ORDER_H
#define ORDER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Order {
    char id[20];
    char sender[20];
    char sender_addr[100];
    char receiver[50];
    char receiver_addr[100];
    char description[100];
    double weight;
    char status[20];
} Order;

void create_order(Order* order, const char* id, const char* sender, const char* sender_addr, const char* receiver, const char* receiver_addr, const char* description, double weight, const char* status);
int print_order(Order* order);
void free_order(Order* order);
double calc_price(Order* order);

#define DEC_UPDATE_ORDER_STR(__PROPERTY) \
void update_order_##__PROPERTY(Order* order, const char* __PROPERTY)

DEC_UPDATE_ORDER_STR(id);
DEC_UPDATE_ORDER_STR(sender);
DEC_UPDATE_ORDER_STR(sender_addr);
DEC_UPDATE_ORDER_STR(receiver);
DEC_UPDATE_ORDER_STR(receiver_addr);
DEC_UPDATE_ORDER_STR(description);
DEC_UPDATE_ORDER_STR(status);

inline void update_order_weight(Order* order, double weight) {
    order->weight = weight;
}


#endif // ORDER_H
