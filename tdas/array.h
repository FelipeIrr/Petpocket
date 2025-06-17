#ifndef ARRAY_H
#define ARRAY_H

#include <stdbool.h>
#include <stddef.h>  

typedef struct Array Array;

Array* array_create(size_t initial_capacity);

void array_push_back(Array* arr, void* element);

void* array_get(Array* arr, size_t index);

void array_set(Array* arr, size_t index, void* element);

size_t array_size(Array* arr);

void array_destroy(Array* arr);

#endif
