#include "raylib.h"
#include "rlgl.h"
#include <math.h>
#include "../tdas/list.h"
#include "../tdas/map.h"
#include "../tdas/array.h"
#ifndef BOTONES_H
#define BOTONES_H

typedef enum { //MODULARIZAR ESTRUCTURAS
    COMIDA,
    ASPECTO
} TipoItem;

typedef struct Item {
    char* nombre;        
    TipoItem tipo; 
    int precio;
    int valor_energetico;
    Texture2D aspecto;  // si es tipo aspecto  
} Item;

typedef struct Escenario {
    char* nombreEscenario;
    Texture2D imagen_fondo;
    int req_energia;
    int req_monedas;
    Map* tienda; //Mapa para la tienda
} Escenario;

typedef struct Mascota {
    char* nombre;
    int energia;
    int monedas;
    Escenario* escenario_actual;
    List* inventario; // ítems en inventario
    Texture2D aspecto_actual;
} Mascota;


Mascota* crearMascota();


#endif