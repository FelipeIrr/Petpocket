#ifndef MAP_H
#define MAP_H
#include "list.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  void *key;
  void *value;
} Pair;

typedef struct Map Map;

Map * createMap(long capacity);

void insertMap(Map * table, char * key, void * value);

void eraseMap(Map * table, char * key);

Pair * searchMap(Map * table, char * key);

Pair * firstMap(Map * table);

Pair * nextMap(Map * table);

void enlarge(Map * map);

void destroyMap(Map * table);

int map_size(Map *map);

#endif /* MAP_H */