#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "map.h"


typedef struct Map Map;
int enlarge_called=0;

struct Map {
    Pair ** buckets;
    long size; 
    long capacity; 
    long current; 
};

//--------------------------------------------------------------

Pair * createPair( char * key,  void * value) {
    Pair * new = (Pair *)malloc(sizeof(Pair));
    if(new == NULL) return NULL; //cambio

    new->key = strdup(key);
    if(new->key == NULL) {
        free(new);
        return NULL; //cambio
    }
    
    new->value = value;
    return new;
}

long hash( char * key, long capacity) {
    unsigned long hash = 0;
     char * ptr;
    for (ptr = key; *ptr != '\0'; ptr++) {
        hash += hash*32 + tolower(*ptr);
    }
    return hash%capacity;
}

int is_equal(void *key1, void *key2) {
    if (key1 == NULL || key2 == NULL) return 0;

    char *str1 = (char *)key1;
    char *str2 = (char *)key2;

    if (str1 == NULL || str2 == NULL) return 0; 
    return strcmp(str1, str2) == 0 ? 1 : 0;
}

void insertMap(Map *map, char *key, void *value) {
    if ((double)map->size / map->capacity >= 0.7) {
        enlarge(map);
    }

    long pos = hash(key, map->capacity);

    while (map->buckets[pos] != NULL) {
        if (map->buckets[pos]->key != NULL && is_equal(map->buckets[pos]->key, key)) return;
        pos = (pos + 1) % map->capacity; 
    }

    Pair *new = createPair(key, value);
    map->buckets[pos] = new;
    map->size++;
}

void enlarge(Map *map) {
    enlarge_called = 1; 
    
    size_t new_capacity = map->capacity * 2;
    Pair **new_buckets = (Pair **)malloc(new_capacity * sizeof(Pair *));
    if (new_buckets == NULL) return; 
    for (size_t i = 0; i < new_capacity; i++) {
        new_buckets[i] = NULL;
    }

    for (size_t i = 0; i < map->capacity; i++) {
        if (map->buckets[i] != NULL && map->buckets[i]->key != NULL) { 
            long pos = hash(map->buckets[i]->key, new_capacity);
            
            size_t attempts = 0;
            while (new_buckets[pos] != NULL) {
                pos = (pos + 1) % new_capacity;
                if (++attempts > new_capacity) return; 
            }

            new_buckets[pos] = map->buckets[i];
        }
    }

    free(map->buckets); 
    map->buckets = new_buckets;
    map->capacity = new_capacity;
}

Map *createMap(long capacity) {
    Map *map = (Map *)malloc(sizeof(Map));
    if (map == NULL) return NULL; // Verificar asignación

    map->buckets = (Pair **)malloc(capacity * sizeof(Pair *));
    if (map->buckets == NULL) { // Manejo de error en la asignación de buckets
        free(map);
        return NULL;
    }

    for (size_t i = 0; i < capacity; i++) {
        map->buckets[i] = NULL;
    }

    map->capacity = capacity;
    map->size = 0;
    map->current = -1;

    return map;
}

void eraseMap(Map *map, char *key) {
    size_t pos = hash(key, map->capacity);

    while (map->buckets[pos] != NULL) {
        if (map->buckets[pos]->key != NULL && is_equal(map->buckets[pos]->key, key)) {
            free(map->buckets[pos]->key); 
            free(map->buckets[pos]); 
            map->buckets[pos] = NULL; 
            map->size--;
            return;
        }
        pos = (pos + 1) % map->capacity;
    }
}

Pair *searchMap(Map *map, char *key) {
    if (map == NULL || key == NULL) return NULL; 

    size_t pos = hash(key, map->capacity);
    size_t start_pos = pos; 
    while (map->buckets[pos] != NULL) {
        if (map->buckets[pos]->key != NULL && is_equal(map->buckets[pos]->key, key)) {
            map->current = pos;
            return map->buckets[pos];
        }
        pos = (pos + 1) % map->capacity;
        if (pos == start_pos) return NULL; 
    }
    return NULL;
}

Pair * firstMap(Map *map) {
    if (map == NULL || map->buckets == NULL || map->capacity <= 0) {
        if (map != NULL) map->current = -1;
        return NULL;
    }

    for (size_t i = 0; i < map->capacity; i++) {
        if (map->buckets[i] != NULL && map->buckets[i]->key != NULL) {
            map->current = i;
            return map->buckets[i];
        }
    }
    map->current = -1;
    return NULL;
}

Pair *nextMap(Map *map) {
    if (map == NULL || map->buckets == NULL || map->current == -1 || map->capacity <= 0) {
        if (map != NULL) map->current = -1;
        return NULL;
    }
    for (size_t i = map->current + 1; i < map->capacity; i++) {
        if (map->buckets[i] != NULL && map->buckets[i]->key != NULL) {
            map->current = i;
            return map->buckets[i];
        }
    }
    map->current = -1; 
    return NULL;
}

void destroyMap(Map *map) {
    if (map == NULL) return;

    for (size_t i = 0; i < map->capacity; i++) {
        if (map->buckets[i] != NULL) {
            free(map->buckets[i]->key);  
            free(map->buckets[i]);       
        }
    }
    free(map->buckets);  
    free(map);           
}

int map_size(Map *map) {
    int count = 0;
    for (Pair *p = firstMap(map); p != NULL; p = nextMap(map))
        count++;
    return count;
}