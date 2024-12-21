#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <order.h>
#include <logger.h>
#include <args.h>
#include <config.h>

#include "menu.h"
#include "operation.h"
#include "color.h"

int main(int argc, char* argv[]) {
    log_message(LOG_INFO, "订单管理系统启动");
    init_console();
    
    log_message(LOG_DEBUG, "初始化日志系统 [默认级别:WARNING]");
    init_logger(NULL);
    set_log_level(LOG_WARNING);

    log_message(LOG_DEBUG, "开始解析命令行参数 [参数数量:%d]", argc - 1);
    ArgumentParser* parser = create_argument_parser();
    if (parser == NULL) {
        log_message(LOG_ERROR, "系统启动失败：无法创建参数解析器");
        return 1;
    }

    add_argument(parser, "c", "配置文件", 0);
    add_argument(parser, "l", "日志级别", 0);
    parse_arguments(parser, argc, argv);

    const char* config_file = get_argument_value(parser, "c");
    if (config_file == NULL) {
        config_file = "config.ini";
        log_message(LOG_INFO, "使用默认配置文件 [文件名:%s]", config_file);
    } else {
        log_message(LOG_INFO, "使用指定配置文件 [文件名:%s]", config_file);
    }

    log_message(LOG_DEBUG, "解析程序路径");
    char resolved_path[1024];
    #ifdef _WIN32
        char* last_slash = strrchr(argv[0], '\\');
        if (last_slash) {
            size_t dir_len = last_slash - argv[0];
            strncpy(resolved_path, argv[0], dir_len);
            resolved_path[dir_len] = '\0';
            SetCurrentDirectory(resolved_path);
            log_message(LOG_DEBUG, "设置当前工作目录 [目录:%s]", resolved_path);
        }
    #else
        char* last_slash = strrchr(argv[0], '/');
        if (last_slash) {
            size_t dir_len = last_slash - argv[0];
            strncpy(resolved_path, argv[0], dir_len);
            resolved_path[dir_len] = '\0';
            chdir(resolved_path);
            log_message(LOG_DEBUG, "设置当前工作目录 [目录:%s]", resolved_path);
        }
    #endif

    if (config_file[0] != '/' && !(config_file[0] && config_file[1] == ':')) {
        char abs_path[1024];
        #ifdef _WIN32
            if (_fullpath(abs_path, config_file, sizeof(abs_path)) != NULL) {
                config_file = strdup(abs_path);
                log_message(LOG_DEBUG, "转换配置文件为绝对路径 [路径:%s]", config_file);
            }
        #else
            if (realpath(config_file, abs_path) != NULL) {
                config_file = strdup(abs_path);
                log_message(LOG_DEBUG, "转换配置文件为绝对路径 [路径:%s]", config_file);
            }
        #endif
    }

    log_message(LOG_DEBUG, "创建配置管理器");
    Config* config = create_config(config_file);
    if (config == NULL) {
        log_message(LOG_ERROR, "系统启动失败：无法创建配置管理器");
        return 1;
    }

    if (load_config(config) != 0) {
        log_message(LOG_WARNING, "无法加载配置文件，将使用默认配置");
    }

    const char* log_file = get_config_value(config, "System", "log_file");
    if (log_file == NULL) {
        log_file = "order_system.log";
        set_config_value(config, "System", "log_file", log_file);
        save_config(config);
        log_message(LOG_INFO, "使用默认日志文件 [文件名:%s]", log_file);
    }

    const char* log_level = get_argument_value(parser, "l");
    if (log_level == NULL) {
        log_level = "DEBUG";
        log_message(LOG_INFO, "使用默认日志级别 [级别:%s]", log_level);
    } else {
        log_message(LOG_INFO, "使用指定日志级别 [级别:%s]", log_level);
    }

    log_message(LOG_DEBUG, "重新初始化日志系统 [文件:%s]", log_file);
    init_logger(log_file);
    if (strcmp(log_level, "DEBUG") == 0) {
        set_log_level(LOG_DEBUG);
    } else if (strcmp(log_level, "INFO") == 0) {
        set_log_level(LOG_INFO);
    } else if (strcmp(log_level, "WARNING") == 0) {
        set_log_level(LOG_WARNING);
    } else if (strcmp(log_level, "ERROR") == 0) {
        set_log_level(LOG_ERROR);
    } else {
        set_log_level(LOG_DEBUG);
    }

    const char* order_file = get_config_value(config, "System", "order_file");
    if (order_file == NULL) {
        order_file = "orders.txt";
        set_config_value(config, "System", "order_file", order_file);
        save_config(config);
        log_message(LOG_INFO, "使用默认订单文件 [文件名:%s]", order_file);
    }

    log_message(LOG_DEBUG, "加载订单数据 [文件名:%s]", order_file);
    Order_array* order_array = load_orders(order_file);
    if (order_array == NULL) {
        log_message(LOG_WARNING, "无法加载订单文件，创建空订单数组");
        order_array = create_order_array(0);
    }

    log_message(LOG_DEBUG, "初始化操作模块");
    init_operation(order_array, config);

    log_message(LOG_INFO, "系统初始化完成，进入主循环");
    while (1) {
        print_menu();
        int op = get_menu_option();
        handle_menu_option(op);
    }

    return 0;
}
