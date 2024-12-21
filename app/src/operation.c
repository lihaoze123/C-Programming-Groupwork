#include <operation.h>

#include "color.h"

static Order_array* order_array = NULL;
static Config* config = NULL;

void init_operation(Order_array* orders, Config* cfg) {
    order_array = orders;
    config = cfg;
}

void save() {
    log_message(LOG_INFO, "保存订单");
    
    const char* order_file = get_config_value(config, "System", "order_file");
    if (order_file == NULL) {
        order_file = "orders.txt";
        set_config_value(config, "System", "order_file", order_file);
        save_config(config);
    }
    
    save_orders(order_array, order_file);
    cprintf(COLOR_GREEN, "保存成功\n");
}

void print() {
    log_message(LOG_INFO, "打印所有订单");

    if (order_array->size == 0) {
        cprintf(COLOR_RED, "没有订单\n");
        return;
    }

    cprintf(COLOR_BLUE, "\n=== 订单列表 ===\n");
    cprintf(COLOR_GREEN, "订单数量: %zu\n", order_array->size);
    cprintf(COLOR_BLUE, "------------------------\n");
    set_text_color(COLOR_WHITE);
    print_orders(order_array);
    cprintf(COLOR_BLUE, "------------------------\n");
}

void exit_system() {
    log_message(LOG_INFO, "退出系统");
    cprintf(COLOR_YELLOW, "正在退出系统...\n");
    free_order_array(order_array);
    free_config(config);
    exit(0);
}

void add() {
    log_message(LOG_INFO, "添加订单");
    cprintf(COLOR_GREEN, "请输入订单信息：\n");
    cprintf(COLOR_GREEN, "(订单号，寄件人，寄件地址，收件人，收件地址，描述，重量，状态)\n");

    Order order;
    if (!get_input_order(&order)) {
        log_message(LOG_ERROR, "输入订单信息失败");
        cprintf(COLOR_RED, "输入订单信息失败\n");
        return;
    }

    if (!add_order(order_array, &order)) {
        log_message(LOG_ERROR, "添加订单失败");
        cprintf(COLOR_RED, "添加订单失败\n");
    } else {
        cprintf(COLOR_GREEN, "添加订单成功\n");
    }
}

Order* find_order() {
    log_message(LOG_INFO, "查找订单");
    cprintf(COLOR_BLUE, "请输入查找方式：\n");
    cprintf(COLOR_GREEN, "(1)：精确查找\n");
    cprintf(COLOR_GREEN, "(2)：模糊查找\n");
    cprintf(COLOR_RED, "(0)：返回\n");

    Order* order = NULL;

    int op;
    scanf("%d", &op);

    if (op == 1) {
        cprintf(COLOR_BLUE, "请输入查找方式：\n");
        cprintf(COLOR_GREEN, "(1)：订单号\n");
        cprintf(COLOR_GREEN, "(2)：寄件人\n");
        cprintf(COLOR_GREEN, "(3)：收件人\n");

        int op;
        scanf("%d", &op);

        switch (op) {
            case 1:
                cprintf(COLOR_WHITE, "请输入订单号：");
                char id[64];
                scanf("%s", id);
                order = get_order_by_id(order_array, id);
                break;
            case 2:
                cprintf(COLOR_WHITE, "请输入寄件人：");
                char sender[64];
                scanf("%s", sender);
                order = get_order_by_sender(order_array, sender);
                break;
            case 3:
                cprintf(COLOR_WHITE, "请输入收件人：");
                char receiver[64];
                scanf("%s", receiver);
                order = get_order_by_receiver(order_array, receiver);
                break;
            default:
                cprintf(COLOR_RED, "无效的选项\n");
                break;
        }
    } else if (op == 2) {
        cprintf(COLOR_WHITE, "请输入关键词：");
        char keyword[64];
        scanf("%s", keyword);
        order = fuzzy_search_order(order_array, keyword);
    } else if (op == 0) {
        return NULL;
    } else {
        cprintf(COLOR_RED, "无效的选项\n");
        return NULL;
    }

    if (order == NULL) {
        cprintf(COLOR_RED, "未找到订单\n");
    } else {
        cprintf(COLOR_BLUE, "\n====================\n");
        set_text_color(COLOR_WHITE);
        print_order(order);
        cprintf(COLOR_BLUE, "\n====================\n");
    }

    return order;
}

void delete_order(Order* order) {
    log_message(LOG_INFO, "删除订单:%s", order->id);

    remove_order(order_array, order->id);
    cprintf(COLOR_GREEN, "删除成功\n");
}

void delete() {
    log_message(LOG_INFO, "删除订单");

    Order* order = find_order();
    if (order == NULL) {
        return;
    }

    delete_order(order);
}

void modify_order(Order* order) {
    log_message(LOG_INFO, "修改订单:%s", order->id);

    cprintf(COLOR_BLUE, "请输入要修改的订单信息：\n");
    cprintf(COLOR_GREEN, "(1): 寄件人\n");
    cprintf(COLOR_GREEN, "(2): 寄件地址\n");
    cprintf(COLOR_GREEN, "(3): 收件人\n");
    cprintf(COLOR_GREEN, "(4): 收件地址\n");
    cprintf(COLOR_GREEN, "(5): 描述\n");
    cprintf(COLOR_GREEN, "(6): 重量\n");
    cprintf(COLOR_GREEN, "(7): 状态\n");

    int op;
    scanf("%d", &op);

    switch (op) {
        case 1:
            cprintf(COLOR_WHITE, "请输入新的寄件人：");
            scanf("%s", order->sender);
            break;
        case 2:
            cprintf(COLOR_WHITE, "请输入新的寄件地址：");
            scanf("%s", order->sender_addr);
            break;
        case 3:
            cprintf(COLOR_WHITE, "请输入新的收件人：");
            scanf("%s", order->receiver);
            break;
        case 4:
            cprintf(COLOR_WHITE, "请输入新的收件地址：");
            scanf("%s", order->receiver_addr);
            break;
        case 5:
            cprintf(COLOR_WHITE, "请输入新的描述：");
            scanf("%s", order->description);
            break;
        case 6:
            cprintf(COLOR_WHITE, "请输入新的重量：");
            scanf("%lf", &order->weight);
            break;
        case 7:
            cprintf(COLOR_WHITE, "请输入新的状态：");
            scanf("%s", order->status);
            break;
        default:
            cprintf(COLOR_RED, "无效的选项\n");
            break;
    }
}

void modify() {
    log_message(LOG_INFO, "修改订单");

    Order* order = find_order();
    if (order == NULL) {
        return;
    }

    modify_order(order);
}

void query() {
    log_message(LOG_INFO, "查询订单");

    Order* order = find_order();
    if (order == NULL) {
        return;
    }

    cprintf(COLOR_BLUE, "请输入要对该订单进行的操作：\n");
    cprintf(COLOR_RED, "(1): 删除\n");
    cprintf(COLOR_GREEN, "(2): 修改\n");
    cprintf(COLOR_RED, "(0): 返回\n");

    int op;
    scanf("%d", &op);

    switch (op) {
        case 1:
            delete_order(order);
            break;
        case 2:
            modify_order(order);
            break;
        case 0:
            return;
        default:
            cprintf(COLOR_RED, "无效的选项\n");
            break;
    }
}

void sort() {
    log_message(LOG_INFO, "排序订单");
    cprintf(COLOR_BLUE, "请选择排序方式：\n");
    cprintf(COLOR_GREEN, "(1): 按订单号排序\n");
    cprintf(COLOR_GREEN, "(2): 按寄件人排序\n");
    cprintf(COLOR_GREEN, "(3): 按收件人排序\n");
    cprintf(COLOR_GREEN, "(4): 按重量排序\n");
    
    int op;
    scanf("%d", &op);
    
    switch(op) {
        case 1:
            sort_orders(order_array, comp_by_id);
            break;
        case 2:
            sort_orders(order_array, comp_by_sender);
            break;
        case 3:
            sort_orders(order_array, comp_by_receiver);
            break;
        case 4:
            sort_orders(order_array, comp_by_weight);
            break;
        default:
            cprintf(COLOR_RED, "无效的选项\n");
            return;
    }
    print();
}

void statistics() {
    log_message(LOG_INFO, "统计订单");
    
    double total_weight = 0;
    double total_price = 0;
    
    for(size_t i = 0; i < order_array->size; i++) {
        Order* order = order_at(order_array, i);
        total_weight += order->weight;
        total_price += calc_price(order);
    }
    
    cprintf(COLOR_BLUE, "\n=== 订单统计 ===\n");
    cprintf(COLOR_GREEN, "订单总数: %zu\n", order_array->size);
    cprintf(COLOR_GREEN, "总重量: %.2f kg\n", total_weight);
    cprintf(COLOR_GREEN, "总金额: ￥%.2f\n", total_price);
}

void export_csv() {
    log_message(LOG_INFO, "导出CSV");
    
    const char* order_csv = get_config_value(config, "System", "order_csv");
    if (order_csv == NULL) {
        order_csv = "orders.csv";
        set_config_value(config, "System", "order_csv", order_csv);
        save_config(config);
    }
    
    FILE* fp = fopen(order_csv, "w");
    if(!fp) {
        cprintf(COLOR_RED, "无法创建文件\n");
        return;
    }
    
    fprintf(fp, "订单号,寄件人,寄件地址,收件人,收件地址,描述,重量,状态\n");
    
    for(size_t i = 0; i < order_array->size; i++) {
        Order* order = order_at(order_array, i);
        fprintf(fp, "%s,%s,%s,%s,%s,%s,%.2f,%s\n",
            order->id, order->sender, order->sender_addr,
            order->receiver, order->receiver_addr,
            order->description, order->weight, order->status);
    }
    
    fclose(fp);
    cprintf(COLOR_GREEN, "导出成功: %s\n", order_csv);
}

void import_csv() {
    log_message(LOG_INFO, "导入CSV");
    
    const char* order_csv = get_config_value(config, "System", "order_csv");
    if (order_csv == NULL) {
        order_csv = "orders.csv";
        set_config_value(config, "System", "order_csv", order_csv);
        save_config(config);
    }
    
    FILE* fp = fopen(order_csv, "r");
    if(!fp) {
        cprintf(COLOR_RED, "找不到文件: %s\n", order_csv);
        return;
    }
    
    char line[1024];
    fgets(line, sizeof(line), fp);
    
    while(fgets(line, sizeof(line), fp)) {
        Order order;
        char* token = strtok(line, ",");
        if(!token) continue;
        
        order.id = strdup(token);
        
        token = strtok(NULL, ",");
        if(!token) continue;
        order.sender = strdup(token);
        
        token = strtok(NULL, ",");
        if(!token) continue;
        order.sender_addr = strdup(token);
        
        token = strtok(NULL, ",");
        if(!token) continue;
        order.receiver = strdup(token);
        
        token = strtok(NULL, ",");
        if(!token) continue;
        order.receiver_addr = strdup(token);
        
        token = strtok(NULL, ",");
        if(!token) continue;
        order.description = strdup(token);
        
        token = strtok(NULL, ",");
        if(!token) continue;
        order.weight = atof(token);
        
        token = strtok(NULL, ",\n");
        if(!token) continue;
        order.status = strdup(token);
        
        add_order(order_array, &order);
    }
    
    fclose(fp);
    cprintf(COLOR_GREEN, "导入成功\n");
}

void help() {
    log_message(LOG_INFO, "显示帮助");
    
    cprintf(COLOR_BLUE, "\n=== 系统使用帮助 ===\n");
    cprintf(COLOR_GREEN, "1. 添加订单: 添加新的订单信息\n");
    cprintf(COLOR_GREEN, "2. 删除订单: 根据订单号删除订单\n");
    cprintf(COLOR_GREEN, "3. 修改订单: 修改已有订单的信息\n");
    cprintf(COLOR_GREEN, "4. 查询订单: 支持精确查找和模糊查找\n");
    cprintf(COLOR_GREEN, "5. 打印订单: 显示所有订单信息\n");
    cprintf(COLOR_GREEN, "6. 保存订单: 保存到文件\n");
    cprintf(COLOR_GREEN, "7. 排序订单: 支持多种排序方式\n");
    cprintf(COLOR_GREEN, "8. 订单统计: 统计订单总数和总金额\n");
    cprintf(COLOR_GREEN, "9. 导出CSV: 将订单导出为CSV格式\n");
    cprintf(COLOR_GREEN, "10. 导入CSV: 从CSV文件导入订单\n");
    cprintf(COLOR_RED, "0. 退出系统: 退出程序\n");
}