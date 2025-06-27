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
    char* ruta_imagen; // Ruta de la imagen del ítem
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
    List* inventario; // ítems en inventario
    Texture2D aspecto_actual;
    Escenario* escenarioActual; // Escenario actual de la mascota
} Mascota;


Mascota* crearMascota();

void reiniciar(Mascota**);

Array* cargarEscenarios();

void cargarItemsTienda(Map* tienda);

void crearTienda(Escenario* escenario);

void mostrarTienda(Mascota* mascota, Escenario* escenario);

void cambiarEscenario(Mascota* mascota, Array* escenarios);

#endif