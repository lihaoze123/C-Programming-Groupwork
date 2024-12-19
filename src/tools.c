#include "tools.h"

int read_string(char* s, size_t size, FILE *stream) {
    if (stream == NULL)
        return 0;

    return ~fscanf(stream, "%s", s);
}

int read_line(char* s, size_t size, FILE *stream) {
    if (stream == NULL)
        return 0;

    char* res = fgets(s, size, stream);

    if (res == NULL) {
        return 0;
    }

    return 1;
}

int parse_order_line(const char* line, Order* order) {
    return ~sscanf(line, "%s %s %s %s %s %s %lf %s", 
           order->id,
           order->sender,
           order->sender_addr,
           order->receiver,
           order->receiver_addr,
           order->description,
           &order->weight,
           order->status
           );
}

int get_file_order(Order *order, FILE *stream) {
    if (stream == NULL)
        return 0;

    char buf[512];
    if (!read_line(buf, 512, stream)) {
        return 0;
    }
    return parse_order_line(buf, order);
}

int get_input_order(Order *order) {
    return get_file_order(order, stdin);
}

int load_orders(Order_array *array, const char *filename) {
    FILE *fp = fopen(filename, "r");

    if (fp == NULL) {
        return 0;
    }

    Order tmp;
    while (get_file_order(&tmp, fp)) {
        if (!add_order(array, &tmp)) {
            fclose(fp);
            return 0;
        }
    }

    fclose(fp);

    return 1;
}

int save_orders(Order_array *array, const char *filename) {
    if (array == NULL || filename == NULL) {
        return 0;
    }

    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        return 0;
    }

    int flag = 1;
    char buf[512];

    for (size_t i = 0; i < array->size && flag; ++i) {
        Order *order = at(array, i);
        if (!sprint_order(order, buf) || 
            !fputs(buf, fp) ||
            !fputc('\n', fp)) {
            flag = 0;
        }
    }

    fclose(fp);
    return flag;
}

int edit_distance(const char* s1, const char* s2) {
    if (s1 == NULL || s2 == NULL) {
        return -1;
    }
    
    int n = strlen(s1);
    int m = strlen(s2);
    
    if (n == 0) return m;
    if (m == 0) return n;
    
    int dp[n + 1][m + 1];
    
    for (int i = 0; i <= n; ++ i) {
        dp[i][0] = i;
    }
    for (int j = 0; j <= m; ++ j) {
        dp[0][j] = j;
    }
    
    for (int i = 1; i <= n; ++ i) {
        for (int j = 1; j <= m; ++ j) {
            if (s1[i - 1] == s2[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1];
            } else {
                int min = dp[i - 1][j - 1];
                if (dp[i - 1][j] < min) min = dp[i - 1][j];
                if (dp[i][j - 1] < min) min = dp[i][j - 1];
                dp[i][j] = min + 1;
            }
        }
    }
    
    return dp[n][m];
}