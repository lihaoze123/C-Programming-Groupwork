#ifndef ARRAY_H
#define ARRAY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <logger.h>

typedef struct Array {
    void* data;
    size_t size;
    size_t capacity;
    size_t element_size;
} Array;

Array* create_array(size_t size, size_t element_size);
int reserve_array(Array* array, size_t new_capacity);
int add_element(Array* array, void* element);
int remove_element(Array* array, size_t index);
void* at(Array* array, size_t index);
void free_array(Array* array);

#endif
