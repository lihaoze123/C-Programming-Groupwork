#ifndef ORDER_ARRAY_H
#define ORDER_ARRAY_H

#include <stdio.h>
#include <string.h>
#include "order.h"

typedef struct Order_array {
    Order* orders;
    size_t size;
    size_t capacity;
} Order_array;

Order_array* create_order_array(size_t initial_capacity);
void reserve_capacity(Order_array *array);
int add_order(Order_array *array, const Order* order);
int remove_order(Order_array *array, const char* id);
Order* get_order(const Order_array *array, const char *order_id);
void print_orders(const Order_array *array);
void free_order_array(Order_array *array);

#endif // ORDER_ARRAY_H
