#include "menu.h"
#include "color.h"

const MenuOption MENU_OPTIONS[] = {
    {0, "退出系统", exit_system},
    {1, "添加订单", add},
    {2, "删除订单", delete},
    {3, "修改订单", modify},
    {4, "查询订单", query},
    {5, "打印所有订单", print},
    {6, "保存订单", save},
};

const size_t MENU_OPTIONS_COUNT = sizeof(MENU_OPTIONS) / sizeof(MenuOption);

void print_menu() {
    cprint(COLOR_BLUE, "\n=== 订单管理系统 ===\n");
    
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
        
        cprint(color, "%d: %s\n", MENU_OPTIONS[i].code, MENU_OPTIONS[i].description);
    }
    
    cprint(COLOR_WHITE, "请输入要进行的操作: ");
}

int get_menu_option() {
    int option;
    scanf("%d", &option);
    clear_line(stdin);

    if (option < 0 || option >= MENU_OPTIONS_COUNT) {
        cprint(COLOR_RED, "无效操作\n");
        return -1;
    }

    return option;
}

void handle_menu_option(int option) {
    if (option == -1) {
        return;
    }

    MENU_OPTIONS[option].operation();
}