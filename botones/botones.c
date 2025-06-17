#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"
#include <math.h>
#include "tdas/list.h"
#include "tdas/map.h"
#include "tdas/array.h"
#define EnergiaMax 100;

typedef enum {
    COMIDA,
    ROPA
} TipoItem;

typedef struct Item {
    char* nombre;         
    TipoItem tipo;
    int precio;
    int valor_energetico;   // solo aplica si tipo == COMIDA
    char* tipo_prenda;      // solo aplica si tipo == ROPA
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
    Item* sombrero;
    Item* camisa;
    Item* pantalones;
    Item* zapatos;
} Mascota;

Mascota* crearMascota(const char* nombre, Escenario* escenarioInicial) {
    Mascota* m = malloc(sizeof(Mascota)); //falta que usuario introduzca el nombre de su mascota.
    m->nombre = strdup(nombre);
    m->energia = EnergiaMax; //energía inicial = Máxima
    m->monedas = 0;  // Monedas iniciales
    m->escenario_actual = escenarioInicial;
    m->inventario = list_create(); // Lista vacía
    m->sombrero = NULL;
    m->camisa = NULL;
    m->pantalones = NULL;
    m->zapatos = NULL;
    return m;
}

void mostrarMenu(Mascota* mascota) {
    int opcion;
    do {
        printf("\n--- Menú Principal ---\n");
        printf("1. Alimentar mascota\n");
        printf("2. Jugar minijuego\n");
        printf("3. Ir a la tienda\n");
        printf("4. Cambiar ropa/accesorios\n");
        printf("5. Cambiar de escenario\n");
        printf("6. Opciones del sistema\n");
        printf("0. Salir\n");
        scanf("%d", &opcion);

        switch (opcion) {
            case 1: alimentarMascota(mascota); break;
            case 2: jugarMinijuego(mascota); break;
            case 3: irATienda(mascota); break;
            case 4: cambiarRopa(mascota); break;
            case 5: cambiarEscenario(mascota); break;
            case 6: opcionesSistema(&mascota); break;
        }
    } while (opcion != 0);
}

void alimentarMascota(Mascota* m) {
    Item* item = (Item*)firstList(m->inventario);  // Primer ítem

    while (item != NULL) {
        if (item->tipo == COMIDA) {
            if (m->energia < 100) {
                m->energia += item->valor_energetico;
                if (m->energia > 100) m->energia = 100;
                list_remove(m->inventario, item); // Elimina la comida consumida
                printf("La mascota ha comido y ahora tiene %d de energía.\n", m->energia);
            } else {
                printf("La energía ya está al máximo.\n");
            }
            return; // Termina después de consumir una comida
        }
        item = (Item*)nextList(m->inventario);
    }

    printf("No tienes comida en el inventario.\n");
}

void jugarMinijuego(Mascota* m) {
    if (m->energia <= 0) {
        printf("La mascota está demasiado cansada para jugar.\n");
        return;
    }

    int k = 5; // Ejemplo: 5 notas
    int monedasGanadas = 0;

    for (int i = 0; i < k; i++) {
        printf("Nota %d: presiona ENTER\n", i+1);
        getchar(); // simula entrada de nota
        monedasGanadas += 5;
    }

    if (m->energia == 100) monedasGanadas += 10;

    m->monedas += monedasGanadas;
    m->energia -= 20;
    if (m->energia < 0) m->energia = 0;

    printf("Ganaste %d monedas. Energía restante: %d\n", monedasGanadas, m->energia);
}





