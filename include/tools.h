#ifndef TOOLS_H
#define TOOLS_H

#include <stdio.h>
#include <stdlib.h>

#include "order.h"
#include "order_array.h"

int read_string(char* s, size_t size, FILE *stream);
int read_line(char* s, size_t size, FILE *stream);

int parse_order_line(const char* line, Order* order);
int get_input_order(Order *order);
int get_file_order(Order *order, FILE *stream);

int load_orders(Order_array *array, const char *filename);
int save_orders(Order_array *array, const char *filename);

int edit_distance(const char* s1, const char* s2);

#endif // TOOLS_H
