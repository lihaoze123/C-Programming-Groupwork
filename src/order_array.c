#include "order_array.h"

Order_array* create_order_array(size_t capacity) {
    if (capacity == 0) {
        log_message(LOG_ERROR, "capacity 不能为 0");
        return NULL;
    }

    Order_array* res = (Order_array*) malloc(sizeof(Order_array));
    if (res == NULL) {
        log_message(LOG_ERROR, "无法分配 Order_array 内存");
        return NULL;
    }

    res->orders = (Order*) malloc(capacity * sizeof(Order));
    if (res->orders == NULL) {
        log_message(LOG_ERROR, "无法分配 orders 数组内存");
        free(res);
        return NULL;
    }

    res->size = 0;
    res->capacity = capacity;
    return res;
}

int reserve_capacity(Order_array *array, size_t new_capacity) {
    if (new_capacity < array->capacity) {
        log_message(LOG_INFO, "new_capacity 小于当前 capacity");
        return 1;
    }

    size_t t = array->size == 0 ? 1 : array->size;
    while (t < new_capacity) {
        t *= 2;
    }

    Order* new_ptr = (Order*) realloc(array->orders, t * sizeof(Order)); 
    if (new_ptr == NULL) {
        log_message(LOG_ERROR, "无法分配内存");
        return 0;
    }

    array->orders = new_ptr;
    array->capacity = t;

    return 1;
}

int add_order(Order_array *array, const Order* order) {
    if(!reserve_capacity(array, array->size + 1)) {
        log_message(LOG_ERROR, "无法添加订单");
        return 0;
    }

    memcpy(&(array->orders[array->size]), order, sizeof(Order));
    array->size ++;

    return 1;
}

Order* at(Order_array *array, size_t index) {
    if (index > array->size) {
        log_message(LOG_ERROR, "index 大于 size");
        return NULL;
    }
    return &array->orders[index];
}

int fprint_orders(const Order_array *array, FILE *stream) {
    if (array == NULL)
        return 0;

    sort_orders(array, comp_by_id);

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
        log_message(LOG_ERROR, "size 为 0");
        return 0;
    }

    Order* find = get_order_by_id(array, id);
    if (find == NULL) {
        log_message(LOG_ERROR, "找不到订单");
        return 0;
    }

    size_t pos = find - array->orders;
    Order* new_ptr = (Order*) malloc((array->size - 1) * sizeof(Order));
    if (new_ptr == NULL) {
        log_message(LOG_ERROR, "无法分配内存");
        return 0;
    }

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
    if (array == NULL) {
        log_message(LOG_ERROR, "array 为 NULL");
        return;
    }

    free(array->orders);
    free(array);
}

int is_sorted(void* arr, size_t nmemb, size_t size, int (*comp)(const void*, const void*)) {
    if (arr == NULL) {
        log_message(LOG_ERROR, "arr 为 NULL");
        return 0;
    }

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
    if (array == NULL) {
        log_message(LOG_ERROR, "array 为 NULL");
        return;
    }

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
        if (strcmp(at(array, mid)->__PROPERTY, value) <= 0) { \
            lo = mid; \
        } else { \
            hi = mid - 1; \
        } \
    } \
    if (strcmp(at(array, lo)->__PROPERTY, value) == 0) { \
        return at(array, lo); \
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

    Order* min = at(array, 0);
    int min_dist = 1e9;
    char str[512];

    for (int i = 0; i < array->size; ++ i) {
        str[0] = '\0';
        Order* cur = at(array, i);
        
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