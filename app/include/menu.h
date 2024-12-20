#ifndef MENU_H
#define MENU_H

#include <operation.h>

typedef void (*Operation)();

typedef struct MenuOption {
    int code;
    const char* description;
    Operation operation;
} MenuOption;

extern const MenuOption MENU_OPTIONS[];
extern const size_t MENU_OPTIONS_COUNT;

void print_menu();
int get_menu_option();
void handle_menu_option(int option);

#endif // MENU_H