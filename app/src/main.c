#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <order.h>
#include "menu.h"
#include "operation.h"

int main(int argc, char* argv[]) {
    init_logger("order_system.log");

    Order_array* order_array = load_orders("orders.txt");
    if (order_array == NULL) {
        order_array = create_order_array(1);
    }

    init_operation(order_array);

    while (1) {
        print_menu();

        int op = get_menu_option();
        log_message(LOG_INFO, "选择操作: %d", op);

        handle_menu_option(op);
    }

    return 0;
}
