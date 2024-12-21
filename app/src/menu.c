#include "menu.h"
#include "color.h"
#include <logger.h>

const MenuOption MENU_OPTIONS[] = {
    {0, "退出系统", exit_system},
    {1, "添加订单", add},
    {2, "删除订单", delete},
    {3, "修改订单", modify},
    {4, "查询订单", query},
    {5, "打印所有订单", print},
    {6, "保存订单", save},
    {7, "排序订单", sort},
    {8, "订单统计", statistics},
    {9, "导出CSV", export_csv},
    {10, "导入CSV", import_csv},
    {11, "帮助", help}
};

const size_t MENU_OPTIONS_COUNT = sizeof(MENU_OPTIONS) / sizeof(MenuOption);

void print_menu() {
    log_message(LOG_DEBUG, "开始显示主菜单");
    cprintf(COLOR_BLUE, "\n=== 订单管理系统 ===\n");
    
    for (size_t i = 0; i < MENU_OPTIONS_COUNT; i++) {
        WORD color;
        switch (MENU_OPTIONS[i].code) {
            case 0: 
                color = COLOR_RED;
                break;
            case 1: 
                color = COLOR_GREEN;
                break;
            case 2: 
                color = COLOR_RED;
                break;
            case 6: 
                color = COLOR_YELLOW;
                break;
            default: 
                color = COLOR_CYAN;
                break;
        }
        
        cprintf(color, "%d: %s\n", MENU_OPTIONS[i].code, MENU_OPTIONS[i].description);
        log_message(LOG_DEBUG, "显示菜单项 [代码:%d, 描述:%s]", 
                   MENU_OPTIONS[i].code, MENU_OPTIONS[i].description);
    }
    
    cprintf(COLOR_WHITE, "请输入要进行的操作: ");
    log_message(LOG_DEBUG, "主菜单显示完成");
}

int get_menu_option() {
    log_message(LOG_DEBUG, "等待用户输入菜单选项");
    
    int option;
    scanf("%d", &option);
    clear_line(stdin);

    if (option < 0 || option >= MENU_OPTIONS_COUNT) {
        log_message(LOG_WARNING, "用户输入无效选项 [输入值:%d]", option);
        cprintf(COLOR_RED, "无效操作\n");
        return -1;
    }

    log_message(LOG_INFO, "用户选择菜单选项 [选项:%d, 描述:%s]", 
                option, MENU_OPTIONS[option].description);
    return option;
}

void handle_menu_option(int option) {
    if (option == -1) {
        log_message(LOG_DEBUG, "跳过无效的菜单选项");
        return;
    }

    log_message(LOG_INFO, "开始处理菜单选项 [选项:%d, 描述:%s]", 
                option, MENU_OPTIONS[option].description);
    MENU_OPTIONS[option].operation();
    log_message(LOG_DEBUG, "菜单选项处理完成 [选项:%d]", option);
}