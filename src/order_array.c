#include "order_array.h"

int fprint_orders(Order_array *array, FILE *stream) {
    if (array == NULL)
        return 0;

    sort_orders(array, comp_by_id);

    for (int i = 0; i < array->size; ++ i) {
        fprint_order(order_at(array, i), stream);
        fprintf(stream, "\n");
    }
    return 1;
}

int print_orders(Order_array *array) {
    return fprint_orders(array, stdout);
}

int remove_order(Order_array *array, const char* id) {
    if (array->size == 0) {
        log_message(LOG_ERROR, "size 为 0");
        return 0;
    }

    Order* find = get_order_by_id(array, id);
    if (find == NULL) {
        log_message(LOG_ERROR, "找不到订单");
        return 0;
    }

    size_t index = find - (Order*)(array->data);
    remove_element(array, index);

    return 1;
}

void sort_orders(Order_array *array, int (*comp)(const void*, const void*)) {
    if (array == NULL) {
        log_message(LOG_ERROR, "array 为 NULL");
        return;
    }

    if (is_sorted(array->data, array->size, sizeof(Order), comp)) {
        return;
    }
    qsort(array->data, array->size, sizeof(Order), comp);
}

#define DEF_COMP_BY_STR(__PROPERTY) \
int comp_by_##__PROPERTY(const void* lhs, const void* rhs) { \
    return strcmp(lhs, rhs); \
}

DEF_COMP_BY_STR(id);
DEF_COMP_BY_STR(sender);
DEF_COMP_BY_STR(sender_addr);
DEF_COMP_BY_STR(receiver);
DEF_COMP_BY_STR(receiver_addr);
DEF_COMP_BY_STR(description);
DEF_COMP_BY_STR(status);

int comp_by_weight(const void* lhs, const void* rhs) {
    if (lhs == NULL || rhs == NULL) {
        log_message(LOG_ERROR, "lhs 或 rhs 为 NULL");
        return 0;
    }

    double arg1 = *(const double*)(lhs);
    double arg2 = *(const double*)(rhs);

    if (arg1 < arg2) return -1;
    if (arg1 > arg2) return 1;
    return 0;
}


#define DEF_GET_ORDER_BY_STR(__PROPERTY) \
Order* get_order_by_##__PROPERTY(Order_array *array, const char *value) { \
    if (array == NULL) { \
        log_message(LOG_ERROR, "array 为 NULL"); \
        return NULL; \
    } \
\
    sort_orders(array, comp_by_##__PROPERTY); \
    size_t lo = 0, hi = array->size - 1; \
    while (lo < hi) { \
        size_t mid = (lo + hi + 1) / 2; \
        if (strcmp(order_at(array, mid)->__PROPERTY, value) <= 0) { \
            lo = mid; \
        } else { \
            hi = mid - 1; \
        } \
    } \
    if (strcmp(order_at(array, lo)->__PROPERTY, value) == 0) { \
        return order_at(array, lo); \
    } else { \
        log_message(LOG_ERROR, "不存在的%s: %s", #__PROPERTY, value); \
        return NULL; \
    } \
}

DEF_GET_ORDER_BY_STR(id);
DEF_GET_ORDER_BY_STR(sender);
DEF_GET_ORDER_BY_STR(receiver);

Order* fuzzy_search_order(Order_array *array, const char *keyword) {
    if (array == NULL) {
        log_message(LOG_ERROR, "array 为 NULL");
        return NULL;
    }

    Order* min = order_at(array, 0);
    int min_dist = 1e9;
    char str[512];

    for (int i = 0; i < array->size; ++ i) {
        str[0] = '\0';
        Order* cur = order_at(array, i);
        
        strcat(str, cur->id);
        strcat(str, cur->sender);
        strcat(str, cur->receiver);
        strcat(str, cur->sender_addr);
        strcat(str, cur->receiver_addr);
        strcat(str, cur->description);
        strcat(str, cur->status);
    
        int dist = edit_distance(str, keyword);
        if (dist < min_dist) {
            min_dist = dist;
            min = cur;
        }
    }

    return min;
}