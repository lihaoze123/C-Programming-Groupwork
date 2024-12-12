#ifndef ORDER_H
#define ORDER_H

typedef struct Order {
    char id[20];
    char sender[20];
    char sender_addr[100];
    char receiver[50];
    char receiver_addr[100];
    char description[100];
    double weight;
    char status[20];
} Order;

void create_order(Order* order, const char* id, const char* sender, const char* sender_addr, const char* receiver, const char* receiver_addr, const char* description[100], double weight, const char* status);
void print_order(Order* order);
void free_order(Order* order);
double calc_price(Order* order);

#define DEF_UPDATE_ORDER(__TYPE, __PROPERTY) \
void update_order_##__PROPERTY(Order* order, __TYPE __PROPERTY)

DEF_UPDATE_ORDER(const char*, id);
DEF_UPDATE_ORDER(const char*, sender);
DEF_UPDATE_ORDER(const char*, sender_addr);
DEF_UPDATE_ORDER(const char*, receiver);
DEF_UPDATE_ORDER(const char*, receiver_addr);
DEF_UPDATE_ORDER(const char*, description);
DEF_UPDATE_ORDER(const char*, status);
DEF_UPDATE_ORDER(double, weight);


#endif // ORDER_H
