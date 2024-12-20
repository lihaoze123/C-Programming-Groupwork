#ifndef ORDER_STRUCT_H
#define ORDER_STRUCT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Order {
    char id[256];
    char sender[256];
    char sender_addr[256];
    char receiver[256];
    char receiver_addr[256];
    char description[256];
    double weight;
    char status[256];
} Order;

void create_order(Order* order, const char* id, const char* sender, const char* sender_addr, const char* receiver, const char* receiver_addr, const char* description, double weight, const char* status);

int sprint_order(Order* order, char* s);
int fprint_order(Order* order, FILE *stream);
int print_order(Order* order);

void free_order(Order* order);
double calc_price(Order* order);

#define UPDATE_ORDER_STR(__OBJECT, __PROPERTY, __NEW) \
    do { \
        strcpy((__OBJECT).__PROPERTY, __NEW); \
    } while(0)

#define UPDATE_ORDER_EQ(__OBJECT, __PROPERTY, __NEW) \
    do { \
        (__OBJECT).__PROPERTY = __NEW; \
    } while(0)

#endif // ORDER_STRUCT_H