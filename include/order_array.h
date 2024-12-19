#ifndef ORDER_ARRAY_H
#define ORDER_ARRAY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "order.h"
#include "logger.h"

typedef struct Order_array {
    Order* orders;
    size_t size, capacity;
} Order_array;

Order_array* create_order_array(size_t capacity);
int reserve_capacity(Order_array *array, size_t new_capacity);
int add_order(Order_array *array, const Order* order);
int remove_order(Order_array *array, const char* id);
void free_order_array(Order_array *array);

Order* at(Order_array *array, size_t index);

#include "tools.h"
Order* fuzzy_search_order(Order_array *array, const char *keyword);

#define DEC_GET_ORDER_BY_STR(__PROPERTY) \
Order* get_order_by_##__PROPERTY(Order_array *array, const char *value);

DEC_GET_ORDER_BY_STR(id);
DEC_GET_ORDER_BY_STR(sender);
DEC_GET_ORDER_BY_STR(receiver);

int fprint_orders(const Order_array *array, FILE *stream);
int print_orders(const Order_array *array);

#define DEC_COMP_BY_STR(__PROPERTY) \
int comp_by_##__PROPERTY(const void* lhs, const void* rhs);

DEC_COMP_BY_STR(id);
DEC_COMP_BY_STR(sender);
DEC_COMP_BY_STR(sender_addr);
DEC_COMP_BY_STR(receiver);
DEC_COMP_BY_STR(receiver_addr);
DEC_COMP_BY_STR(description);
DEC_COMP_BY_STR(status);

int comp_by_weight(const void* lhs, const void* rhs);

int is_sorted(void* arr, size_t nmemb, size_t size, int (*comp)(const void*, const void*));
void sort_orders(Order_array *array, int (*comp)(const void*, const void*));


#endif // ORDER_ARRAY_H
