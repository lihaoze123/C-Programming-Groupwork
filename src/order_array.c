#include "order_array.h"

int fprint_orders(Order_array *array, FILE *stream) {
    if (array == NULL) {
        log_message(LOG_ERROR, "打印订单失败：数组为NULL");
        return 0;
    }

    log_message(LOG_INFO, "开始打印订单列表 [订单数:%zu]", array->size);
    sort_orders(array, comp_by_id);

    for (int i = 0; i < array->size; ++ i) {
        Order* order = order_at(array, i);
        fprint_order(order, stream);
        fprintf(stream, "\n");
    }

    log_message(LOG_INFO, "订单列表打印完成");
    return 1;
}

int print_orders(Order_array *array) {
    return fprint_orders(array, stdout);
}

int remove_order(Order_array *array, const char* id) {
    if (array->size == 0) {
        log_message(LOG_ERROR, "删除订单失败：订单数组为空");
        return 0;
    }

    Order* find = get_order_by_id(array, id);
    if (find == NULL) {
        log_message(LOG_ERROR, "删除订单失败：找不到订单 [ID:%s]", id);
        return 0;
    }

    size_t index = find - (Order*)(array->data);
    if (!remove_element(array, index)) {
        log_message(LOG_ERROR, "删除订单失败：移除元素失败 [ID:%s, 索引:%zu]", id, index);
        return 0;
    }

    log_message(LOG_INFO, "成功从数组中移除订单 [ID:%s, 索引:%zu]", id, index);
    return 1;
}

void sort_orders(Order_array *array, int (*comp)(const void*, const void*)) {
    if (array == NULL) {
        log_message(LOG_ERROR, "排序订单失败：数组为NULL");
        return;
    }

    if (is_sorted(array->data, array->size, sizeof(Order), comp)) {
        log_message(LOG_DEBUG, "订单数组已经有序，无需排序 [订单数:%zu]", array->size);
        return;
    }

    log_message(LOG_INFO, "开始对订单数组排序 [订单数:%zu]", array->size);
    qsort(array->data, array->size, sizeof(Order), comp);
    log_message(LOG_INFO, "订单数组排序完成");
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
        log_message(LOG_ERROR, "按重量比较失败：参数为NULL");
        return 0;
    }

    double arg1 = *(const double*)(lhs);
    double arg2 = *(const double*)(rhs);

    log_message(LOG_DEBUG, "比较重量 [左值:%.2f, 右值:%.2f]", arg1, arg2);

    if (arg1 < arg2) return -1;
    if (arg1 > arg2) return 1;
    return 0;
}


#define DEF_GET_ORDER_BY_STR(__PROPERTY) \
Order* get_order_by_##__PROPERTY(Order_array *array, const char *value) { \
    if (array == NULL) { \
        log_message(LOG_ERROR, "按%s搜索订单失败：数组为NULL", #__PROPERTY); \
        return NULL; \
    } \
    \
    log_message(LOG_INFO, "开始按%s搜索订单 [查找值:%s]", #__PROPERTY, value); \
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
        Order* result = order_at(array, lo); \
        log_message(LOG_INFO, "成功找到订单 [%s:%s, ID:%s]", #__PROPERTY, value, result->id); \
        return result; \
    } else { \
        log_message(LOG_WARNING, "未找到订单 [%s:%s]", #__PROPERTY, value); \
        return NULL; \
    } \
}

DEF_GET_ORDER_BY_STR(id);
DEF_GET_ORDER_BY_STR(sender);
DEF_GET_ORDER_BY_STR(receiver);

Order* fuzzy_search_order(Order_array *array, const char *keyword) {
    if (array == NULL) {
        log_message(LOG_ERROR, "模糊搜索订单失败：数组为NULL");
        return NULL;
    }

    if (array->size == 0) {
        log_message(LOG_WARNING, "模糊搜索订单：数组为空");
        return NULL;
    }

    log_message(LOG_INFO, "开始模糊搜索订单 [关键词:%s]", keyword);

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
            log_message(LOG_DEBUG, "找到更好的匹配 [ID:%s, 编辑距离:%d]", min->id, min_dist);
        }
    }

    log_message(LOG_INFO, "模糊搜索完成 [关键词:%s, 最佳匹配ID:%s, 编辑距离:%d]", 
                keyword, min->id, min_dist);
    return min;
}