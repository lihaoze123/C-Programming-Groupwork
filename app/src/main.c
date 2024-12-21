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
    init_console();
    init_logger(NULL);

    ArgumentParser* parser = create_argument_parser();
    if (parser == NULL) {
        log_message(LOG_ERROR, "无法创建参数解析器");
        return 1;
    }

    add_argument(parser, "c", "配置文件", 0);
    parse_arguments(parser, argc, argv);

    const char* config_file = get_argument_value(parser, "c");
    if (config_file == NULL) {
        config_file = "config.ini";
    }

    char resolved_path[1024];
    #ifdef _WIN32
        char* last_slash = strrchr(argv[0], '\\');
        if (last_slash) {
            size_t dir_len = last_slash - argv[0];
            strncpy(resolved_path, argv[0], dir_len);
            resolved_path[dir_len] = '\0';
            SetCurrentDirectory(resolved_path);
        }
    #else
        char* last_slash = strrchr(argv[0], '/');
        if (last_slash) {
            size_t dir_len = last_slash - argv[0];
            strncpy(resolved_path, argv[0], dir_len);
            resolved_path[dir_len] = '\0';
            chdir(resolved_path);
        }
    #endif

    // 如果配置文件路径是相对路径,转换为绝对路径
    if (config_file[0] != '/' && !(config_file[0] && config_file[1] == ':')) {
        char abs_path[1024];
        #ifdef _WIN32
            if (_fullpath(abs_path, config_file, sizeof(abs_path)) != NULL) {
                config_file = strdup(abs_path);
            }
        #else
            if (realpath(config_file, abs_path) != NULL) {
                config_file = strdup(abs_path);
            }
        #endif
    }

    Config* config = create_config(config_file);
    if (config == NULL) {
        log_message(LOG_ERROR, "无法创建配置管理器");
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
    }

    init_logger(log_file);

    const char* order_file = get_config_value(config, "System", "order_file");
    if (order_file == NULL) {
        order_file = "orders.txt";
        set_config_value(config, "System", "order_file", order_file);
        save_config(config);
    }

    Order_array* order_array = load_orders(order_file);
    if (order_array == NULL) {
        order_array = create_order_array(0);
    }

    init_operation(order_array, config);

    while (1) {
        print_menu();

        int op = get_menu_option();
        log_message(LOG_INFO, "选择操作: %d", op);

        handle_menu_option(op);
    }

    return 0;
}
