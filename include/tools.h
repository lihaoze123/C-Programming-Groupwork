#ifndef TOOLS_H
#define TOOLS_H

#include <stdio.h>
#include <stdlib.h>

#include "order_struct.h"
#include "order_array.h"
#include "logger.h"

int read_string(char* s, size_t size, FILE *stream);
int read_line(char* s, size_t size, FILE *stream);
int clear_line(FILE *stream);

int parse_order_line(const char* line, Order* order);
int get_input_order(Order *order);
int get_file_order(Order *order, FILE *stream);

Order_array* load_orders(const char *filename);
int save_orders(Order_array *array, const char *filename);

int edit_distance(const char* s1, const char* s2);

int is_sorted(void* arr, size_t nmemb, size_t size, int (*comp)(const void*, const void*));

#endif // TOOLS_H