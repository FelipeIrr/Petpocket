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
    TipoItem tipo;  // tipo de item
    int precio;
    int valor_energetico; // si es tipo comida
    Texture2D aspecto;  // si es tipo aspecto  
    char* ruta_imagen; // Ruta de la imagen del ítem
} Item;

typedef struct Escenario {
    char* nombreEscenario;
    Texture2D imagen_fondo; // Imagen de fondo del escenario
    int req_energia; // Requisitos de energía para acceder al escenario
    int req_monedas; // Requisitos de monedas para acceder al escenario
    Map* tienda; //Mapa para la tienda
} Escenario;

typedef struct Mascota {
    char* nombre;
    int energia;
    int monedas;
    List* inventario; // ítems en inventario
    Texture2D aspecto_actual; // Aspecto actual de la mascota
    Escenario* escenarioActual; // Escenario actual de la mascota
} Mascota;


Mascota* crearMascota(); // crea una mascota y le asigna un nombre

void reiniciar(Mascota**); // reinicia la mascota y su inventario

Array* cargarEscenarios(); // carga los escenarios desde un archivo CSV y devuelve un Array de Escenarios

void cargarItemsTienda(Map* tienda); // carga los ítems de la tienda desde un archivo CSV y los inserta en el mapa de la tienda

void crearTienda(Escenario* escenario); // crea la tienda para un escenario dado

void mostrarTienda(Mascota* mascota, Escenario* escenario); // muestra la tienda del escenario actual de la mascota

void cambiarEscenario(Mascota* mascota, Array* escenarios); // cambia el escenario actual de la mascota a otro escenario disponible en el Array de escenarios

void cambiarAspectoMascota(Mascota* mascota); // cambia el aspecto de la mascota a uno seleccionado por el usuario

void alimentarMascota(Mascota* mascota); // alimenta a la mascota con un ítem de comida del inventario

#endif