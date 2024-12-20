#ifndef ORDER_ARRAY_H
#define ORDER_ARRAY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <logger.h>
#include <array.h>

#include "order_struct.h"

typedef Array Order_array;

static inline Order_array* create_order_array(size_t size) {
    return create_array(size, sizeof(Order));
}

static inline int reserve_capacity(Order_array *array, size_t new_capacity) {
    return reserve_array(array, new_capacity);
}

static inline int add_order(Order_array *array, const Order* order) {
    return add_element(array, (void*) order);
}

static inline Order* order_at(Order_array *array, size_t index) {
    return (Order*) at(array, index);
}

static inline void free_order_array(Order_array *array) {
    free_array(array);
}

int remove_order(Order_array *array, const char* id);

#include "tools.h"
Order* fuzzy_search_order(Order_array *array, const char *keyword);

#define DEC_GET_ORDER_BY_STR(__PROPERTY) \
Order* get_order_by_##__PROPERTY(Order_array *array, const char *value);

DEC_GET_ORDER_BY_STR(id);
DEC_GET_ORDER_BY_STR(sender);
DEC_GET_ORDER_BY_STR(receiver);

int fprint_orders(Order_array *array, FILE *stream);
int print_orders(Order_array *array);

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

void sort_orders(Order_array *array, int (*comp)(const void*, const void*));


#endif // ORDER_ARRAY_H
