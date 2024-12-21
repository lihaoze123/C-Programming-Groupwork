#include "tools.h"

int read_string(char* s, size_t size, FILE *stream) {
    if (stream == NULL) {
        log_message(LOG_ERROR, "读取字符串失败：输入流为NULL");
        return 0;
    }

    int result = ~fscanf(stream, "%s", s);
    if (!result) {
        log_message(LOG_ERROR, "读取字符串失败：fscanf返回错误");
    }
    return result;
}

int read_line(char* s, size_t size, FILE *stream) {
    if (stream == NULL) {
        log_message(LOG_ERROR, "读取行失败：输入流为NULL");
        return 0;
    }

    char* res = fgets(s, size, stream);
    if (res == NULL) {
        log_message(LOG_ERROR, "读取行失败：fgets返回NULL");
        return 0;
    }

    log_message(LOG_DEBUG, "成功读取行 [内容:%s]", s);
    return 1;
}

int clear_line(FILE *stream) {
    if (stream == NULL) {
        log_message(LOG_ERROR, "清除行失败：输入流为NULL");
        return 0;
    }

    while (fgetc(stream) != '\n');
    log_message(LOG_DEBUG, "成功清除行");
    return 1;
}

int parse_order_line(const char* line, Order* order) {
    if (!line || !order) {
        log_message(LOG_ERROR, "解析订单行失败：参数无效");
        return 0;
    }

    log_message(LOG_DEBUG, "开始解析订单行 [内容:%s]", line);

    char id[256];
    char sender[256];
    char sender_addr[256];
    char receiver[256];
    char receiver_addr[256];
    char description[256];
    char status[256];
    double weight;

    int res = ~sscanf(line, "%s %s %s %s %s %s %lf %s", 
           id,
           sender,
           sender_addr,
           receiver,
           receiver_addr,
           description,
           &weight,
           status
           );

    if (!res) {
        log_message(LOG_ERROR, "解析订单行失败：格式不匹配");
        return 0;
    }

    create_order(order, id, sender, sender_addr, receiver, receiver_addr, description, weight, status);
    log_message(LOG_DEBUG, "成功解析订单行 [ID:%s]", id);
    return res;
}

int get_file_order(Order *order, FILE *stream) {
    if (!order || !stream) {
        log_message(LOG_ERROR, "从文件获取订单失败：参数无效");
        return 0;
    }

    char buf[512];
    if (!read_line(buf, 512, stream)) {
        log_message(LOG_ERROR, "从文件获取订单失败：读取行失败");
        return 0;
    }

    int result = parse_order_line(buf, order);
    if (result) {
        log_message(LOG_DEBUG, "成功从文件获取订单 [ID:%s]", order->id);
    } else {
        log_message(LOG_ERROR, "从文件获取订单失败：解析行失败");
    }
    return result;
}

int get_input_order(Order *order) {
    log_message(LOG_INFO, "开始从标准输入获取订单");
    int result = get_file_order(order, stdin);
    if (result) {
        log_message(LOG_INFO, "成功从标准输入获取订单 [ID:%s]", order->id);
    } else {
        log_message(LOG_ERROR, "从标准输入获取订单失败");
    }
    return result;
}

Order_array* load_orders(const char *filename) {
    log_message(LOG_INFO, "开始加载订单文件 [文件名:%s]", filename);

    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        log_message(LOG_ERROR, "加载订单失败：无法打开文件 [文件名:%s]", filename);
        return NULL;
    }

    Order_array* array = create_order_array(0);
    if (!array) {
        log_message(LOG_ERROR, "加载订单失败：创建订单数组失败");
        fclose(fp);
        return NULL;
    }

    size_t count = 0;
    Order tmp;
    while (get_file_order(&tmp, fp)) {
        if (!add_order(array, &tmp)) {
            log_message(LOG_ERROR, "加载订单失败：添加订单失败 [ID:%s]", tmp.id);
            fclose(fp);
            return NULL;
        }
        count++;
        log_message(LOG_DEBUG, "成功加载订单 [ID:%s]", tmp.id);
    }

    fclose(fp);
    log_message(LOG_INFO, "订单文件加载完成 [文件名:%s, 加载数量:%zu]", filename, count);
    return array;
}

int save_orders(Order_array *array, const char *filename) {
    if (!array || !filename) {
        log_message(LOG_ERROR, "保存订单失败：参数无效");
        return 0;
    }

    log_message(LOG_INFO, "开始保存订单文件 [文件名:%s, 订单数:%zu]", filename, array->size);

    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        log_message(LOG_ERROR, "保存订单失败：无法创建文件 [文件名:%s]", filename);
        return 0;
    }

    int flag = 1;
    char buf[512];
    size_t success_count = 0;

    for (size_t i = 0; i < array->size && flag; ++i) {
        Order *order = at(array, i);
        if (!sprint_order(order, buf) || 
            !fputs(buf, fp) ||
            !fputc('\n', fp)) {
            log_message(LOG_ERROR, "保存订单失败：写入订单失败 [ID:%s]", order->id);
            flag = 0;
        } else {
            success_count++;
            log_message(LOG_DEBUG, "成功写入订单 [ID:%s]", order->id);
        }
    }

    fclose(fp);
    if (flag) {
        log_message(LOG_INFO, "订单文件保存完成 [文件名:%s, 保存数量:%zu]", filename, success_count);
    } else {
        log_message(LOG_ERROR, "订单文件保存失败 [文件名:%s, 已保存数量:%zu]", filename, success_count);
    }
    return flag;
}

int edit_distance(const char* s1, const char* s2) {
    if (!s1 || !s2) {
        log_message(LOG_ERROR, "计算编辑距离失败：参数无效");
        return -1;
    }
    
    size_t n = strlen(s1);
    size_t m = strlen(s2);
    
    if (n < m) {
        return edit_distance(s2, s1);
    }
    
    log_message(LOG_DEBUG, "开始计算编辑距离 [字符串1:%s, 字符串2:%s]", s1, s2);
    
    int min_dist = m;
    for (size_t st = 0; st <= n - m; st++) {
        int** dp = (int**) malloc((m + 1) * sizeof(int*));
        for (size_t i = 0; i <= m; i++) {
            dp[i] = (int*) malloc((m + 1) * sizeof(int));
        }
        
        for (size_t i = 0; i <= m; i++) {
            dp[i][0] = i;
            dp[0][i] = i;
        }
        
        for (size_t i = 1; i <= m; i++) {
            for (size_t j = 1; j <= m; j++) {
                if (s1[st + i - 1] == s2[j - 1]) {
                    dp[i][j] = dp[i - 1][j - 1];
                } else {
                    int min = dp[i - 1][j - 1];
                    if (dp[i - 1][j] < min) min = dp[i - 1][j];
                    if (dp[i][j - 1] < min) min = dp[i][j - 1];
                    dp[i][j] = min + 1;
                }
            }
        }
        
        if (dp[m][m] < min_dist) {
            min_dist = dp[m][m];
        }

        // 释放内存
        for (size_t i = 0; i <= m; i++) {
            free(dp[i]);
        }
        free(dp);
    }
    
    log_message(LOG_DEBUG, "编辑距离计算完成 [结果:%d]", min_dist);
    return min_dist;
}

int is_sorted(void* arr, size_t nmemb, size_t size, int (*comp)(const void*, const void*)) {
    if (!arr) {
        log_message(LOG_ERROR, "检查排序状态失败：数组为NULL");
        return 0;
    }

    log_message(LOG_DEBUG, "开始检查数组排序状态 [元素数:%zu]", nmemb);

    for (int i = 0; i < nmemb - 1; ++ i) {
        void* current = (char*) arr + i * size;
        void* next = (char*) arr + (i + 1) * size;

        if (comp(current, next) > 0) {
            log_message(LOG_DEBUG, "数组未排序 [位置:%d]", i);
            return 0;
        }
    }

    log_message(LOG_DEBUG, "数组已排序");
    return 1;
}
