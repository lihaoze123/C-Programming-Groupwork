#include "order_array.h"

Order_array* create_order_array(size_t size) {
    Order_array* res = (Order_array*) malloc(sizeof(Order_array));

    res->orders = (Order*) malloc(size * sizeof(Order));
    res->size = size;

    if (res == NULL || res->orders == NULL) {
        printf("申请内存失败\n");
        free(res);
        return NULL;
    }

    return res;
}

int reserve_capacity(Order_array *array, size_t new_size) {
    if (new_size < array->size) {
        return 0;
    }

    Order* new_ptr = (Order*) realloc(array->orders, new_size * sizeof(Order)); 
    if (new_ptr == NULL) {
        free(new_ptr);
        return 0;
    }

    array->orders = new_ptr;
    array->size = new_size;

    return 1;
}

int add_order(Order_array *array, const Order* order) {
    if(!reserve_capacity(array, array->size + 1)) {
        return 0;
    }

    memcpy(&(array->orders[array->size - 1]), order, sizeof(Order));
    return 1;
}

Order* at(Order_array *array, size_t index) {
    if (index > array->size) {
        return NULL;
    }
    return &array->orders[index];
}

int print_orders(const Order_array *array) {
    if (array == NULL)
        return 0;

    for (int i = 0; i < array->size; ++ i) {
        print_order(at((Order_array*)array, i));
    }
    return 1;
}

Order* get_order(Order_array *array, const char *id) {
    sort_orders(array, comp_by_id);

    size_t lo = 0, hi = array->size - 1;
    while (lo < hi) {
        size_t mid = (lo + hi + 1) / 2;
        if (strcmp(at(array, mid)->id, id) <= 0) {
            lo = mid; 
        } else {
            hi = mid - 1;
        }
    }

    if (strcmp(at(array, lo)->id, id) == 0) {
        return at(array, lo);
    } else {
        printf("不存在的运单号\n");
        return NULL;
    }
}

int remove_order(Order_array *array, const char* id) {
    if (array->size == 0) {
        return 0;
    }

    Order* find = get_order(array, id);
    if (find == NULL) {
        return 0;
    }

    size_t pos = find - array->orders;
    Order* new_ptr = (Order*) malloc((array->size - 1) * sizeof(Order));

    for (int i = 0, j = 0; i < array->size - 1; ++ i) {
        if (i == pos) 
            continue;
        memcpy(&new_ptr[j ++], &array->orders[i], sizeof(Order));
    }

    free(array->orders);

    array->orders = new_ptr;
    array->size --;

    return 1;
}

void free_order_array(Order_array *array) {
    free(array->orders);
    free(array);
}

void sort_orders(Order_array *array, int (*comp)(const void*, const void*)) {
    qsort(array->orders, array->size, sizeof(Order), comp);
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
    double arg1 = *(const double*)(lhs);
    double arg2 = *(const double*)(rhs);

    if (arg1 < arg2) return -1;
    if (arg1 > arg2) return 1;
    return 0;
}

