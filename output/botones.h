#include "raylib.h"
#include "rlgl.h"
#include <math.h>
#include "../tdas/list.h"
#include "../tdas/map.h"
#include "../tdas/array.h"
#ifndef BOTONES_H
#define BOTONES_H

typedef enum {
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
    Escenario* escenario_actual;
    List* inventario; // ítems en inventario
} Mascota;

Mascota* crearMascota(Escenario* escenarioInicial);
//FUNCIONES AUXILIARES DEL JUEGO
//--------------------------------------------------------------------------------------------------------------------------------------

//ALIMENTAR MASCOTA
void alimentarMascota(Mascota* m);
// AVANZAR DE ESCENARIO
void cambiarEscenario(Mascota* mascota, Array* escenarios);
// ACCESO A TIENDA
void tienda(Mascota* mascota);

//OPCIONES DEL JUEGO
void opcionesDelSistema(Mascota** mascota, Escenario* escenario_inicial);



#endif