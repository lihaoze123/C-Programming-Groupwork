#include "order.h"

void create_order(Order* order, const char* id, const char* sender, const char* sender_addr, const char* receiver, const char* receiver_addr, const char* description, double weight, const char* status) {
    strcpy(order->id, id);
    strcpy(order->sender, sender);
    strcpy(order->sender_addr, sender_addr);
    strcpy(order->receiver, receiver);
    strcpy(order->receiver_addr, receiver_addr);
    strcpy(order->description, description);
    strcpy(order->status, status);
    order->weight = weight;
}

int sprint_order(Order* order, char* s) {
    if (order == NULL)
        return 0;

    sprintf(s, "%s %s %s %s %s %s %lf %s", 
           order->id,
           order->sender,
           order->sender_addr,
           order->receiver,
           order->receiver_addr,
           order->description,
           order->weight,
           order->status
           );

    return 1;
}

int fprint_order(Order* order, FILE *stream) {
    if (order == NULL)
        return 0;

    char buf[200];
    sprint_order(order, buf);

    fprintf(stream, "%s", buf);

    return 1;
}

int print_order(Order *order) {
    return fprint_order(order, stdout);
}

double calc_price(Order* order) {
    return 10.0 * order->weight;
}

void free_order(Order* order) {
    free(order);
}
