#ifndef TOOLS_H
#define TOOLS_H

#include <stdio.h>
#include <stdlib.h>
#include "order.h"
#include "order_array.h"

void greeting();

int read_string(char* s, size_t size, FILE *stream);
int read_line(char* s, size_t size, FILE *stream);

int parse_order_line(const char* line, Order* order);
void get_input_order(Order *order);

int load_orders(Order_array *array, const char *filename);
int save_orders(Order_array *array, const char *filename);

#endif // TOOLS_H
