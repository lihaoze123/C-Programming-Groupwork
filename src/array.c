#include "array.h"

Array* create_array(size_t size, size_t element_size) {
    if (size < 0 || element_size < 0) {
        log_message(LOG_ERROR, "size 或 element_size 不能小于 0");
        return NULL;
    }

    Array* array = (Array*) malloc(sizeof(Array));
    if (array == NULL) {
        log_message(LOG_ERROR, "无法分配内存");
        return NULL;
    }

    array->data = malloc(size * element_size);
    if (array->data == NULL && size > 0) {
        log_message(LOG_ERROR, "无法分配内存");
        free(array);
        return NULL;
    }

    array->size = size;
    array->element_size = element_size;
    array->capacity = 0;

    reserve_array(array, size);

    return array;
}

int reserve_array(Array* array, size_t new_capacity) {
    if (new_capacity < array->capacity) {
        log_message(LOG_INFO, "已经预留了足够的空间 %zu < %zu", new_capacity, array->capacity);
        return 1;
    }

    size_t t = array->size == 0 ? 1 : array->size;
    while (t < new_capacity) {
        t *= 2;
    }

    void* new_ptr = realloc(array->data, t * array->element_size);
    if (new_ptr == NULL) {
        log_message(LOG_ERROR, "无法分配内存");
        return 0;
    }

    array->data = new_ptr;
    array->capacity = t;
    return 1;
}

int add_element(Array* array, void* element) {
    if (!reserve_array(array, array->size + 1)) {
        log_message(LOG_ERROR, "无法分配内存");
        return 0;
    }

    char* dest = (char*) array->data + array->size * array->element_size;
    memcpy(dest, element, array->element_size);
    array->size++;
    return 1;
}

void* at(Array* array, size_t index) {
    if (index >= array->size) {
        log_message(LOG_ERROR, "index 超出范围");
        return NULL;
    }
    return (char*) array->data + index * array->element_size;
}

int remove_element(Array* array, size_t index) {
    if (index >= array->size) {
        log_message(LOG_ERROR, "index 超出范围");
        return 0;
    }

    memmove((char*) array->data + index * array->element_size, (char*) array->data + (index + 1) * array->element_size, (array->size - index - 1) * array->element_size);
    array->size--;
    return 1;
}

void free_array(Array* array) {
    free(array->data);
    free(array);
}