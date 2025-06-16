#include "array.h"
#include <stdlib.h>
#include <string.h>

struct Array {
    void** data;
    size_t size;
    size_t capacity;
};

Array* array_create(size_t initial_capacity) {
    Array* arr = malloc(sizeof(Array));
    if (!arr) return NULL;
    arr->data = malloc(sizeof(void*) * initial_capacity);
    arr->size = 0;
    arr->capacity = initial_capacity;
    return arr;
}

void array_push_back(Array* arr, void* element) {
    if (arr->size >= arr->capacity) {
        size_t new_capacity = arr->capacity * 2;
        void** new_data = realloc(arr->data, sizeof(void*) * new_capacity);
        if (!new_data) return;
        arr->data = new_data;
        arr->capacity = new_capacity;
    }
    arr->data[arr->size++] = element;
}

void* array_get(Array* arr, size_t index) {
    if (index >= arr->size) return NULL;
    return arr->data[index];
}

void array_set(Array* arr, size_t index, void* element) {
    if (index < arr->size) {
        arr->data[index] = element;
    }
}

size_t array_size(Array* arr) {
    return arr->size;
}

void array_destroy(Array* arr) {
    if (arr) {
        free(arr->data);
        free(arr);
    }
}
