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

int fprint_orders(const Order_array *array, FILE *stream) {
    if (array == NULL)
        return 0;

    for (int i = 0; i < array->size; ++ i) {
        fprint_order(at((Order_array*)array, i), stream);
        fprintf(stream, "\n");
    }
    return 1;
}

int print_orders(const Order_array *array) {
    return fprint_orders(array, stdout);
}

int remove_order(Order_array *array, const char* id) {
    if (array->size == 0) {
        return 0;
    }

    Order* find = get_order_by_id(array, id);
    if (find == NULL) {
        return 0;
    }

    size_t pos = find - array->orders;
    Order* new_ptr = (Order*) malloc((array->size - 1) * sizeof(Order));

    for (int i = 0, j = 0; i < array->size; ++ i) {
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

int is_sorted(void* arr, size_t nmemb, size_t size, int (*comp)(const void*, const void*)) {
    for (int i = 0; i < nmemb - 1; ++ i) {
        void* current = (char*) arr + i * size;
        void* next = (char*) arr + (i + 1) * size;

        if (comp(current, next) > 0) {
            return 0;
        }
    }

    return 1;
}

void sort_orders(Order_array *array, int (*comp)(const void*, const void*)) {
    if (is_sorted(array->orders, array->size, sizeof(Order), comp)) {
        return;
    }
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


#define DEF_GET_ORDER_BY_STR(__PROPERTY) \
Order* get_order_by_##__PROPERTY(Order_array *array, const char *value) { \
    sort_orders(array, comp_by_##__PROPERTY); \
    size_t lo = 0, hi = array->size - 1; \
    while (lo < hi) { \
        size_t mid = (lo + hi + 1) / 2; \
        if (strcmp(at(array, mid)->__PROPERTY, value) <= 0) { \
            lo = mid; \
        } else { \
            hi = mid - 1; \
        } \
    } \
    if (strcmp(at(array, lo)->__PROPERTY, value) == 0) { \
        return at(array, lo); \
    } else { \
        log_message(stderr, LOG_ERROR, "不存在的%s: %s", #__PROPERTY, value); \
        return NULL; \
    } \
}

DEF_GET_ORDER_BY_STR(id);
DEF_GET_ORDER_BY_STR(sender);
DEF_GET_ORDER_BY_STR(receiver);

Order* fuzzy_search_order(Order_array *array, const char *keyword) {
    Order* min = at(array, 0);
    int min_dist = 1e9;

    for (int i = 0; i < array->size; ++ i) {
        char* str = strcat(at(array, i)->id, at(array, i)->sender);
        str = strcat(str, at(array, i)->receiver);
        str = strcat(str, at(array, i)->sender_addr);
        str = strcat(str, at(array, i)->receiver_addr);
        str = strcat(str, at(array, i)->description);
        str = strcat(str, at(array, i)->status);
    
        if (edit_distance(str, keyword) < min_dist) {
            min_dist = edit_distance(str, keyword);
            min = at(array, i);
        }
    }

    return min;
}