#include "order_struct.h"

void create_order(Order* order, const char* id, const char* sender, const char* sender_addr, const char* receiver, const char* receiver_addr, const char* description, double weight, const char* status) {
    if (!order || !id || !sender || !sender_addr || !receiver || !receiver_addr || !description || !status) {
        log_message(LOG_ERROR, "创建订单失败：参数无效");
        return;
    }

    log_message(LOG_DEBUG, "开始创建订单 [ID:%s]", id);

    order->id = strdup(id);
    order->sender = strdup(sender);
    order->sender_addr = strdup(sender_addr);
    order->receiver = strdup(receiver);
    order->receiver_addr = strdup(receiver_addr);
    order->description = strdup(description);
    order->status = strdup(status);
    order->weight = weight;

    if (!order->id || !order->sender || !order->sender_addr || !order->receiver || 
        !order->receiver_addr || !order->description || !order->status) {
        log_message(LOG_ERROR, "创建订单失败：内存分配失败 [ID:%s]", id);
        return;
    }

    log_message(LOG_DEBUG, "订单创建完成 [ID:%s, 寄件人:%s, 收件人:%s]", 
                order->id, order->sender, order->receiver);
}

int sprint_order(Order* order, char* s) {
    if (!order || !s) {
        log_message(LOG_ERROR, "格式化订单失败：参数无效");
        return 0;
    }

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

    log_message(LOG_DEBUG, "订单格式化完成 [ID:%s, 结果:%s]", order->id, s);
    return 1;
}

int fprint_order(Order* order, FILE *stream) {
    if (!order || !stream) {
        log_message(LOG_ERROR, "打印订单失败：参数无效");
        return 0;
    }

    char buf[200];
    if (!sprint_order(order, buf)) {
        log_message(LOG_ERROR, "打印订单失败：格式化失败 [ID:%s]", order->id);
        return 0;
    }

    if (fprintf(stream, "%s", buf) < 0) {
        log_message(LOG_ERROR, "打印订单失败：写入失败 [ID:%s]", order->id);
        return 0;
    }

    log_message(LOG_DEBUG, "订单打印完成 [ID:%s]", order->id);
    return 1;
}

int print_order(Order *order) {
    if (!order) {
        log_message(LOG_ERROR, "打印订单失败：订单为NULL");
        return 0;
    }

    log_message(LOG_DEBUG, "开始打印订单到标准输出 [ID:%s]", order->id);
    return fprint_order(order, stdout);
}

double calc_price(Order* order) {
    if (!order) {
        log_message(LOG_ERROR, "计算价格失败：订单为NULL");
        return 0.0;
    }

    double price = 10.0 * order->weight;
    log_message(LOG_DEBUG, "订单价格计算完成 [ID:%s, 重量:%.2f, 价格:%.2f]", 
                order->id, order->weight, price);
    return price;
}

void free_order(Order* order) {
    if (!order) {
        log_message(LOG_WARNING, "释放订单失败：订单为NULL");
        return;
    }

    log_message(LOG_DEBUG, "开始释放订单内存 [ID:%s]", order->id);
    
    if (order->id) free(order->id);
    if (order->sender) free(order->sender);
    if (order->sender_addr) free(order->sender_addr);
    if (order->receiver) free(order->receiver);
    if (order->receiver_addr) free(order->receiver_addr);
    if (order->description) free(order->description);
    if (order->status) free(order->status);
    
    free(order);
    log_message(LOG_DEBUG, "订单内存释放完成");
}
