#include <operation.h>

#include "color.h"

static Order_array* order_array = NULL;
static Config* config = NULL;

void init_operation(Order_array* orders, Config* cfg) {
    log_message(LOG_INFO, "开始初始化操作模块");
    if (!orders || !cfg) {
        log_message(LOG_ERROR, "初始化失败：参数无效");
        return;
    }
    order_array = orders;
    config = cfg;
    log_message(LOG_INFO, "操作模块初始化完成 [订单数:%zu]", orders->size);
}

void save() {
    log_message(LOG_INFO, "开始保存订单");
    
    const char* order_file = get_config_value(config, "System", "order_file");
    if (order_file == NULL) {
        order_file = "orders.txt";
        set_config_value(config, "System", "order_file", order_file);
        save_config(config);
        log_message(LOG_INFO, "使用默认订单文件名 [文件名:%s]", order_file);
    }
    
    if (save_orders(order_array, order_file)) {
        log_message(LOG_INFO, "订单保存成功 [文件名:%s, 订单数:%zu]", order_file, order_array->size);
        cprintf(COLOR_GREEN, "保存成功\n");
    } else {
        log_message(LOG_ERROR, "订单保存失败 [文件名:%s]", order_file);
        cprintf(COLOR_RED, "保存失败\n");
    }
}

void print() {
    log_message(LOG_INFO, "开始打印订单列表");

    if (order_array->size == 0) {
        log_message(LOG_WARNING, "打印订单列表：没有订单");
        cprintf(COLOR_RED, "没有订单\n");
        return;
    }

    log_message(LOG_DEBUG, "打印订单列表信息 [订单数:%zu]", order_array->size);
    cprintf(COLOR_BLUE, "\n=== 订单列表 ===\n");
    cprintf(COLOR_GREEN, "订单数量: %zu\n", order_array->size);
    cprintf(COLOR_BLUE, "------------------------\n");
    set_text_color(COLOR_WHITE);
    print_orders(order_array);
    cprintf(COLOR_BLUE, "------------------------\n");
    log_message(LOG_INFO, "订单列表打印完成");
}

void exit_system() {
    log_message(LOG_INFO, "开始退出系统");
    cprintf(COLOR_YELLOW, "正在退出系统...\n");
    
    if (order_array) {
        log_message(LOG_DEBUG, "释放订单数组 [订单数:%zu]", order_array->size);
        free_order_array(order_array);
    }
    
    if (config) {
        log_message(LOG_DEBUG, "释放配置信息 [配置项数:%zu]", config->items->size);
        free_config(config);
    }
    
    log_message(LOG_INFO, "系统退出完成");
    exit(0);
}

void add() {
    log_message(LOG_INFO, "开始添加订单");
    cprintf(COLOR_GREEN, "请输入订单信息：\n");
    cprintf(COLOR_GREEN, "(订单号，寄件人，寄件地址，收件人，收件地址，描述，重量，状态)\n");

    Order order;
    if (!get_input_order(&order)) {
        log_message(LOG_ERROR, "添加订单失败：输入信息无效");
        cprintf(COLOR_RED, "输入订单信息失败\n");
        return;
    }

    log_message(LOG_DEBUG, "尝试添加订单 [ID:%s, 寄件人:%s, 收件人:%s]", 
                order.id, order.sender, order.receiver);

    if (!add_order(order_array, &order)) {
        log_message(LOG_ERROR, "添加订单失败：可能是订单号重复 [ID:%s]", order.id);
        cprintf(COLOR_RED, "添加订单失败\n");
    } else {
        log_message(LOG_INFO, "成功添加订单 [ID:%s, 当前总数:%zu]", order.id, order_array->size);
        cprintf(COLOR_GREEN, "添加订单成功\n");
    }
}

void print_search_menu() {
    cprintf(COLOR_BLUE, "请输入查找方式：\n");
    cprintf(COLOR_GREEN, "(1)：精确查找\n");
    cprintf(COLOR_GREEN, "(2)：模糊查找\n");
    cprintf(COLOR_RED, "(0)：返回\n");
}

void print_exact_search_menu() {
    cprintf(COLOR_BLUE, "请输入查找方式：\n");
    cprintf(COLOR_GREEN, "(1)：订单号\n");
    cprintf(COLOR_GREEN, "(2)：寄件人\n");
    cprintf(COLOR_GREEN, "(3)：收件人\n");
}

Order* exact_search() {
    log_message(LOG_INFO, "开始精确搜索订单");
    print_exact_search_menu();

    int op;
    scanf("%d", &op);

    char input[64];
    Order* order = NULL;

    switch (op) {
        case 1:
            cprintf(COLOR_WHITE, "请输入订单号：");
            scanf("%s", input);
            log_message(LOG_DEBUG, "按订单号搜索 [输入值:%s]", input);
            order = get_order_by_id(order_array, input);
            break;
        case 2:
            cprintf(COLOR_WHITE, "请输入寄件人：");
            scanf("%s", input);
            log_message(LOG_DEBUG, "按寄件人搜索 [输入值:%s]", input);
            order = get_order_by_sender(order_array, input);
            break;
        case 3:
            cprintf(COLOR_WHITE, "请输入收件人：");
            scanf("%s", input);
            log_message(LOG_DEBUG, "按收件人搜索 [输入值:%s]", input);
            order = get_order_by_receiver(order_array, input);
            break;
        default:
            log_message(LOG_WARNING, "无效的搜索选项 [选项:%d]", op);
            cprintf(COLOR_RED, "无效的选项\n");
            break;
    }

    if (order) {
        log_message(LOG_INFO, "精确搜索成功 [ID:%s]", order->id);
    } else {
        log_message(LOG_INFO, "精确搜索未找到结果");
    }
    return order;
}

Order* fuzzy_search() {
    log_message(LOG_INFO, "开始模糊搜索订单");
    cprintf(COLOR_WHITE, "请输入关键词：");
    char keyword[64];
    scanf("%s", keyword);
    log_message(LOG_DEBUG, "模糊搜索关键词 [关键词:%s]", keyword);
    Order* order = fuzzy_search_order(order_array, keyword);
    if (order) {
        log_message(LOG_INFO, "模糊搜索成功 [ID:%s]", order->id);
    } else {
        log_message(LOG_INFO, "模糊搜索未找到结果");
    }
    return order;
}

void print_order_result(Order* order) {
    if (order == NULL) {
        log_message(LOG_WARNING, "打印订单结果：订单为空");
        cprintf(COLOR_RED, "未找到订单\n");
    } else {
        log_message(LOG_INFO, "打印订单信息 [ID:%s]", order->id);
        cprintf(COLOR_BLUE, "\n====================\n");
        set_text_color(COLOR_WHITE);
        print_order(order);
        cprintf(COLOR_BLUE, "\n====================\n");
    }
}

Order* find_order() {
    log_message(LOG_INFO, "开始查找订单");
    print_search_menu();

    int op;
    scanf("%d", &op);
    log_message(LOG_DEBUG, "选择查找方式 [选项:%d]", op);

    Order* order = NULL;

    switch (op) {
        case 1:
            order = exact_search();
            break;
        case 2:
            order = fuzzy_search();
            break;
        case 0:
            log_message(LOG_INFO, "取消查找订单");
            return NULL;
        default:
            log_message(LOG_WARNING, "无效的查找选项 [选项:%d]", op);
            cprintf(COLOR_RED, "无效的选项\n");
            return NULL;
    }

    print_order_result(order);
    return order;
}

void delete_order(Order* order) {
    log_message(LOG_INFO, "开始删除订单 [ID:%s, 寄件人:%s, 收件人:%s]", order->id, order->sender, order->receiver);

    if (remove_order(order_array, order->id)) {
        log_message(LOG_INFO, "成功删除订单 [ID:%s]", order->id);
        cprintf(COLOR_GREEN, "删除成功\n");
    } else {
        log_message(LOG_ERROR, "删除订单失败 [ID:%s]", order->id);
        cprintf(COLOR_RED, "删除失败\n");
    }
}

void delete() {
    log_message(LOG_INFO, "开始删除订单");

    Order* order = find_order();
    if (order == NULL) {
        log_message(LOG_WARNING, "删除订单取消：未找到订单");
        return;
    }

    log_message(LOG_DEBUG, "准备删除订单 [ID:%s, 寄件人:%s, 收件人:%s]", 
                order->id, order->sender, order->receiver);
    delete_order(order);
}

void modify_order(Order* order) {
    log_message(LOG_INFO, "开始修改订单 [ID:%s, 寄件人:%s, 收件人:%s]", order->id, order->sender, order->receiver);

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

    char old_value[256];
    switch (op) {
        case 1:
            strcpy(old_value, order->sender);
            cprintf(COLOR_WHITE, "请输入新的寄件人：");
            scanf("%s", order->sender);
            log_message(LOG_INFO, "修改寄件人 [ID:%s, 旧值:%s, 新值:%s]", order->id, old_value, order->sender);
            break;
        case 2:
            strcpy(old_value, order->sender_addr);
            cprintf(COLOR_WHITE, "请输入新的寄件地址：");
            scanf("%s", order->sender_addr);
            log_message(LOG_INFO, "修改寄件地址 [ID:%s, 旧值:%s, 新值:%s]", order->id, old_value, order->sender_addr);
            break;
        case 3:
            strcpy(old_value, order->receiver);
            cprintf(COLOR_WHITE, "请输入新的收件人：");
            scanf("%s", order->receiver);
            log_message(LOG_INFO, "修改收件人 [ID:%s, 旧值:%s, 新值:%s]", order->id, old_value, order->receiver);
            break;
        case 4:
            strcpy(old_value, order->receiver_addr);
            cprintf(COLOR_WHITE, "请输入新的收件地址：");
            scanf("%s", order->receiver_addr);
            log_message(LOG_INFO, "修改收件地址 [ID:%s, 旧值:%s, 新值:%s]", order->id, old_value, order->receiver_addr);
            break;
        case 5:
            strcpy(old_value, order->description);
            cprintf(COLOR_WHITE, "请输入新的描述：");
            scanf("%s", order->description);
            log_message(LOG_INFO, "修改描述 [ID:%s, 旧值:%s, 新值:%s]", order->id, old_value, order->description);
            break;
        case 6:
            double old_weight = order->weight;
            cprintf(COLOR_WHITE, "请输入新的重量：");
            scanf("%lf", &order->weight);
            log_message(LOG_INFO, "修改重量 [ID:%s, 旧值:%.2f, 新值:%.2f]", order->id, old_weight, order->weight);
            break;
        case 7:
            strcpy(old_value, order->status);
            cprintf(COLOR_WHITE, "请输入新的状态：");
            scanf("%s", order->status);
            log_message(LOG_INFO, "修改状态 [ID:%s, 旧值:%s, 新值:%s]", order->id, old_value, order->status);
            break;
        default:
            log_message(LOG_WARNING, "无效的修改选项 [选项:%d]", op);
            cprintf(COLOR_RED, "无效的选项\n");
            break;
    }
}

void modify() {
    log_message(LOG_INFO, "开始修改订单");

    Order* order = find_order();
    if (order == NULL) {
        log_message(LOG_WARNING, "修改订单取消：未找到订单");
        return;
    }

    log_message(LOG_DEBUG, "准备修改订单 [ID:%s, 寄件人:%s, 收件人:%s]", 
                order->id, order->sender, order->receiver);
    modify_order(order);
}

void query() {
    log_message(LOG_INFO, "开始查询订单");

    Order* order = find_order();
    if (order == NULL) {
        log_message(LOG_WARNING, "查询订单取消：未找到订单");
        return;
    }

    log_message(LOG_DEBUG, "显示订单操作菜单 [ID:%s]", order->id);
    cprintf(COLOR_BLUE, "请输入要对该订单进行的操作：\n");
    cprintf(COLOR_RED, "(1): 删除\n");
    cprintf(COLOR_GREEN, "(2): 修改\n");
    cprintf(COLOR_RED, "(0): 返回\n");

    int op;
    scanf("%d", &op);
    log_message(LOG_DEBUG, "选择订单操作 [选项:%d]", op);

    switch (op) {
        case 1:
            delete_order(order);
            break;
        case 2:
            modify_order(order);
            break;
        case 0:
            log_message(LOG_INFO, "取消订单操作");
            return;
        default:
            log_message(LOG_WARNING, "无效的操作选项 [选项:%d]", op);
            cprintf(COLOR_RED, "无效的选项\n");
            break;
    }
}

void sort() {
    log_message(LOG_INFO, "开始排序订单");
    cprintf(COLOR_BLUE, "请选择排序方式：\n");
    cprintf(COLOR_GREEN, "(1): 按订单号排序\n");
    cprintf(COLOR_GREEN, "(2): 按寄件人排序\n");
    cprintf(COLOR_GREEN, "(3): 按收件人排序\n");
    cprintf(COLOR_GREEN, "(4): 按重量排序\n");
    
    int op;
    scanf("%d", &op);
    log_message(LOG_DEBUG, "选择排序方式 [选项:%d]", op);
    
    switch(op) {
        case 1:
            log_message(LOG_INFO, "执行按订单号排序");
            sort_orders(order_array, comp_by_id);
            break;
        case 2:
            log_message(LOG_INFO, "执行按寄件人排序");
            sort_orders(order_array, comp_by_sender);
            break;
        case 3:
            log_message(LOG_INFO, "执行按收件人排序");
            sort_orders(order_array, comp_by_receiver);
            break;
        case 4:
            log_message(LOG_INFO, "执行按重量排序");
            sort_orders(order_array, comp_by_weight);
            break;
        default:
            log_message(LOG_WARNING, "无效的排序选项 [选项:%d]", op);
            cprintf(COLOR_RED, "无效的选项\n");
            return;
    }
    log_message(LOG_INFO, "排序完成，开始打印结果");
    print();
}

void statistics() {
    log_message(LOG_INFO, "开始统计订单信息");
    
    double total_weight = 0;
    double total_price = 0;
    size_t pending_count = 0;
    size_t processing_count = 0; 
    size_t completed_count = 0;
    
    for(size_t i = 0; i < order_array->size; i++) {
        Order* order = order_at(order_array, i);
        total_weight += order->weight;
        total_price += calc_price(order);
        
        if(strcmp(order->status, "待处理") == 0) {
            pending_count++;
        } else if(strcmp(order->status, "处理中") == 0) {
            processing_count++;
        } else if(strcmp(order->status, "已完成") == 0) {
            completed_count++;
        }
    }
    
    cprintf(COLOR_BLUE, "\n=== 订单统计 ===\n");
    cprintf(COLOR_GREEN, "订单总数: %zu\n", order_array->size);
    cprintf(COLOR_GREEN, "总重量: %.2f kg\n", total_weight);
    cprintf(COLOR_GREEN, "总金额: ￥%.2f\n", total_price);
    cprintf(COLOR_GREEN, "平均重量: %.2f kg\n", order_array->size > 0 ? total_weight / order_array->size : 0);
    cprintf(COLOR_GREEN, "平均金额: ￥%.2f\n", order_array->size > 0 ? total_price / order_array->size : 0);
    cprintf(COLOR_BLUE, "\n=== 订单状态统计 ===\n");
    cprintf(COLOR_GREEN, "待处理订单: %zu (%.1f%%)\n", pending_count, 
            order_array->size > 0 ? pending_count * 100.0 / order_array->size : 0);
    cprintf(COLOR_GREEN, "处理中订单: %zu (%.1f%%)\n", processing_count,
            order_array->size > 0 ? processing_count * 100.0 / order_array->size : 0);
    cprintf(COLOR_GREEN, "已完成订单: %zu (%.1f%%)\n", completed_count,
            order_array->size > 0 ? completed_count * 100.0 / order_array->size : 0);

    log_message(LOG_INFO, "订单统计结果 [总数:%zu, 总重量:%.2f, 总金额:%.2f, 待处理:%zu, 处理中:%zu, 已完成:%zu]", 
        order_array->size, total_weight, total_price, pending_count, processing_count, completed_count);
}

void export_csv() {
    log_message(LOG_INFO, "开始导出CSV文件");
    
    const char* order_csv = get_config_value(config, "System", "order_csv");
    if (order_csv == NULL) {
        order_csv = "orders.csv";
        set_config_value(config, "System", "order_csv", order_csv);
        save_config(config);
        log_message(LOG_INFO, "使用默认CSV文件名 [文件名:%s]", order_csv);
    }
    
    FILE* fp = fopen(order_csv, "w");
    if(!fp) {
        log_message(LOG_ERROR, "创建CSV文件失败 [文件名:%s]", order_csv);
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
    log_message(LOG_INFO, "成功导出CSV文件 [文件名:%s, 订单数:%zu]", order_csv, order_array->size);
    cprintf(COLOR_GREEN, "导出成功: %s\n", order_csv);
}

void import_csv() {
    log_message(LOG_INFO, "开始导入CSV文件");
    
    const char* order_csv = get_config_value(config, "System", "order_csv");
    if (order_csv == NULL) {
        order_csv = "orders.csv";
        set_config_value(config, "System", "order_csv", order_csv);
        save_config(config);
        log_message(LOG_INFO, "使用默认CSV文件名 [文件名:%s]", order_csv);
    }
    
    FILE* fp = fopen(order_csv, "r");
    if(!fp) {
        log_message(LOG_ERROR, "导入CSV失败：无法打开文件 [文件名:%s]", order_csv);
        cprintf(COLOR_RED, "找不到文件: %s\n", order_csv);
        return;
    }
    
    char line[1024];
    fgets(line, sizeof(line), fp);  // 跳过标题行
    
    size_t success_count = 0;
    size_t error_count = 0;
    size_t line_number = 1;  // 从1开始，因为已经读取了标题行

    log_message(LOG_INFO, "开始解析CSV文件内容");
    
    while(fgets(line, sizeof(line), fp)) {
        line_number++;
        Order order;
        char* token = strtok(line, ",");
        if(!token) {
            log_message(LOG_WARNING, "解析CSV行失败：格式错误 [行号:%zu]", line_number);
            error_count++;
            continue;
        }
        
        order.id = strdup(token);
        
        token = strtok(NULL, ",");
        if(!token) {
            log_message(LOG_WARNING, "解析CSV行失败：缺少寄件人 [行号:%zu, ID:%s]", line_number, order.id);
            free(order.id);
            error_count++;
            continue;
        }
        order.sender = strdup(token);
        
        token = strtok(NULL, ",");
        if(!token) {
            log_message(LOG_WARNING, "解析CSV行失败：缺少寄件地址 [行号:%zu, ID:%s]", line_number, order.id);
            free(order.id);
            free(order.sender);
            error_count++;
            continue;
        }
        order.sender_addr = strdup(token);
        
        token = strtok(NULL, ",");
        if(!token) {
            log_message(LOG_WARNING, "解析CSV行失败：缺少收件人 [行号:%zu, ID:%s]", line_number, order.id);
            free(order.id);
            free(order.sender);
            free(order.sender_addr);
            error_count++;
            continue;
        }
        order.receiver = strdup(token);
        
        token = strtok(NULL, ",");
        if(!token) {
            log_message(LOG_WARNING, "解析CSV行失败：缺少收件地址 [行号:%zu, ID:%s]", line_number, order.id);
            free(order.id);
            free(order.sender);
            free(order.sender_addr);
            free(order.receiver);
            error_count++;
            continue;
        }
        order.receiver_addr = strdup(token);
        
        token = strtok(NULL, ",");
        if(!token) {
            log_message(LOG_WARNING, "解析CSV行失败：缺少描述 [行号:%zu, ID:%s]", line_number, order.id);
            free(order.id);
            free(order.sender);
            free(order.sender_addr);
            free(order.receiver);
            free(order.receiver_addr);
            error_count++;
            continue;
        }
        order.description = strdup(token);
        
        token = strtok(NULL, ",");
        if(!token) {
            log_message(LOG_WARNING, "解析CSV行失败：缺少重量 [行号:%zu, ID:%s]", line_number, order.id);
            free(order.id);
            free(order.sender);
            free(order.sender_addr);
            free(order.receiver);
            free(order.receiver_addr);
            free(order.description);
            error_count++;
            continue;
        }
        order.weight = atof(token);
        
        token = strtok(NULL, ",\n");
        if(!token) {
            log_message(LOG_WARNING, "解析CSV行失败：缺少状态 [行号:%zu, ID:%s]", line_number, order.id);
            free(order.id);
            free(order.sender);
            free(order.sender_addr);
            free(order.receiver);
            free(order.receiver_addr);
            free(order.description);
            error_count++;
            continue;
        }
        order.status = strdup(token);
        
        if(add_order(order_array, &order)) {
            log_message(LOG_DEBUG, "成功导入订单 [行号:%zu, ID:%s]", line_number, order.id);
            success_count++;
        } else {
            log_message(LOG_ERROR, "添加订单失败 [行号:%zu, ID:%s]", line_number, order.id);
            error_count++;
        }
    }
    
    fclose(fp);
    log_message(LOG_INFO, "CSV文件导入完成 [总行数:%zu, 成功:%zu, 失败:%zu]", 
                line_number - 1, success_count, error_count);
    cprintf(COLOR_GREEN, "导入完成 [成功:%zu, 失败:%zu]\n", success_count, error_count);
}

void help() {
    log_message(LOG_INFO, "显示系统帮助信息");
    
    cprintf(COLOR_BLUE, "\n=== 系统使用帮助 ===\n");
    const char* help_items[] = {
        "添加订单: 添加新的订单信息",
        "删除订单: 根据订单号删除订单",
        "修改订单: 修改已有订单的信息",
        "查询订单: 支持精确查找和模糊查找",
        "打印订单: 显示所有订单信息",
        "保存订单: 保存到文件",
        "排序订单: 支持多种排序方式",
        "订单统计: 统计订单总数和总金额",
        "导出CSV: 将订单导出为CSV格式",
        "导入CSV: 从CSV文件导入订单",
        "退出系统: 退出程序"
    };

    for (int i = 0; i < 10; i++) {
        cprintf(COLOR_GREEN, "%d. %s\n", i + 1, help_items[i]);
        log_message(LOG_DEBUG, "显示帮助项 [序号:%d, 内容:%s]", i + 1, help_items[i]);
    }
    cprintf(COLOR_RED, "0. %s\n", help_items[10]);
    log_message(LOG_DEBUG, "显示帮助项 [序号:0, 内���:%s]", help_items[10]);
    
    log_message(LOG_INFO, "系统帮助信息显示完成");
}