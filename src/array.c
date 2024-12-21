#include "array.h"

Array* create_array(size_t size, size_t element_size) {
    if (size < 0 || element_size < 0) {
        log_message(LOG_ERROR, "创建数组失败：参数无效 [size:%zu, element_size:%zu]", size, element_size);
        return NULL;
    }

    log_message(LOG_DEBUG, "开始创建数组 [size:%zu, element_size:%zu]", size, element_size);

    Array* array = (Array*) malloc(sizeof(Array));
    if (array == NULL) {
        log_message(LOG_ERROR, "创建数组失败：无法分配数组结构内存");
        return NULL;
    }

    array->data = malloc(size * element_size);
    if (array->data == NULL && size > 0) {
        log_message(LOG_ERROR, "创建数组失败：无法分配数据内存 [size:%zu, element_size:%zu]", size, element_size);
        free(array);
        return NULL;
    }

    array->size = size;
    array->element_size = element_size;
    array->capacity = 0;

    if (!reserve_array(array, size)) {
        log_message(LOG_ERROR, "创建数组失败：预留空间失败 [size:%zu]", size);
        free(array->data);
        free(array);
        return NULL;
    }

    log_message(LOG_DEBUG, "数组创建完成 [size:%zu, capacity:%zu]", array->size, array->capacity);
    return array;
}

int reserve_array(Array* array, size_t new_capacity) {
    if (!array) {
        log_message(LOG_ERROR, "预留数组空间失败：数组为NULL");
        return 0;
    }

    if (new_capacity < array->capacity) {
        log_message(LOG_DEBUG, "数组已有足够空间 [当前容量:%zu, 请求容量:%zu]", array->capacity, new_capacity);
        return 1;
    }

    log_message(LOG_DEBUG, "开始扩展数组空间 [当前容量:%zu, 请求容量:%zu]", array->capacity, new_capacity);

    size_t t = array->size == 0 ? 1 : array->size;
    while (t < new_capacity) {
        t *= 2;
    }

    void* new_ptr = realloc(array->data, t * array->element_size);
    if (new_ptr == NULL) {
        log_message(LOG_ERROR, "预留数组空间失败：内存分配失败 [请求容量:%zu]", t);
        return 0;
    }

    array->data = new_ptr;
    array->capacity = t;
    log_message(LOG_DEBUG, "数组空间扩展完成 [新容量:%zu]", array->capacity);
    return 1;
}

int add_element(Array* array, void* element) {
    if (!array || !element) {
        log_message(LOG_ERROR, "添加元素失败：参数无效");
        return 0;
    }

    log_message(LOG_DEBUG, "开始添加元素 [当前大小:%zu]", array->size);

    if (!reserve_array(array, array->size + 1)) {
        log_message(LOG_ERROR, "添加元素失败：扩展空间失败");
        return 0;
    }

    char* dest = (char*) array->data + array->size * array->element_size;
    memcpy(dest, element, array->element_size);
    array->size++;

    log_message(LOG_DEBUG, "元素添加完成 [新大小:%zu]", array->size);
    return 1;
}

void* at(Array* array, size_t index) {
    if (!array) {
        log_message(LOG_ERROR, "访问元素失败：数组为NULL");
        return NULL;
    }

    if (index >= array->size) {
        log_message(LOG_ERROR, "访问元素失败：索引越界 [索引:%zu, 大小:%zu]", index, array->size);
        return NULL;
    }

    log_message(LOG_DEBUG, "访问数组元素 [索引:%zu]", index);
    return (char*) array->data + index * array->element_size;
}

int remove_element(Array* array, size_t index) {
    if (!array) {
        log_message(LOG_ERROR, "移除元素失败：数组为NULL");
        return 0;
    }

    if (index >= array->size) {
        log_message(LOG_ERROR, "移除元素失败：索引越界 [索引:%zu, 大小:%zu]", index, array->size);
        return 0;
    }

    log_message(LOG_DEBUG, "开始移除元素 [索引:%zu, 当前大小:%zu]", index, array->size);

    memmove((char*) array->data + index * array->element_size, 
            (char*) array->data + (index + 1) * array->element_size, 
            (array->size - index - 1) * array->element_size);
    array->size--;

    log_message(LOG_DEBUG, "元素移除完成 [新大小:%zu]", array->size);
    return 1;
}

void free_array(Array* array) {
    if (!array) {
        log_message(LOG_WARNING, "释放数组失败：数组为NULL");
        return;
    }

    log_message(LOG_DEBUG, "开始释放数组 [大小:%zu, 容量:%zu]", array->size, array->capacity);
    
    if (array->data) {
        free(array->data);
    }
    free(array);
    
    log_message(LOG_DEBUG, "数组释放完成");
}