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
    FILE *fp = fopen(filename, "w");

    if (fp == NULL) {
        return 0;
    }

    char buf[512];
    for (size_t i = 0; i < array->size; ++ i) {
        sprint_order(at(array, i), buf);
        if (!fputs(buf, fp)) {
            fclose(fp);
            return 0;
        }
    }

    fclose(fp);

    return 1;
}
