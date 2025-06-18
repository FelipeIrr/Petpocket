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
    int valor_energetico;   // solo aplica si tipo  COMIDA
    char* tipo_prenda;      // solo aplica si tipo  ROPA
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

Mascota* crearMascotaConInput(Escenario* escenarioInicial) {
    char nombreMascota[32] = "";
    bool nombreIngresado = false;
    bool textBoxEditMode = true;
    Rectangle textBox = { 350, 300, 300, 50 };

    SetMouseCursor(MOUSE_CURSOR_IBEAM);

    while (!nombreIngresado && !WindowShouldClose()) {
        if (CheckCollisionPointRec(GetMousePosition(), textBox)) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) textBoxEditMode = true;
        } else {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) textBoxEditMode = false;
        }

        if (textBoxEditMode) {
            int key = GetCharPressed();
            while (key > 0) {
                if ((key >= 32) && (key <= 125) && (strlen(nombreMascota) < 31)) {
                    int len = strlen(nombreMascota);
                    nombreMascota[len] = (char)key;
                    nombreMascota[len + 1] = '\0';
                }
                key = GetCharPressed();
            }

            if (IsKeyPressed(KEY_BACKSPACE)) {
                int len = strlen(nombreMascota);
                if (len > 0) nombreMascota[len - 1] = '\0';
            }

            if (IsKeyPressed(KEY_ENTER) && strlen(nombreMascota) > 0) {
                nombreIngresado = true;
            }
        }

        //dibujo
        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText("Ingresa el nombre de tu mascota y presiona ENTER:", 250, 240, 20, DARKGRAY);
            DrawRectangleRec(textBox, LIGHTGRAY);
            DrawRectangleLinesEx(textBox, 1, GRAY);
            DrawText(nombreMascota, (int)textBox.x + 5, (int)textBox.y + 15, 20, MAROON);

            if (textBoxEditMode) DrawText("_", (int)textBox.x + 5 + MeasureText(nombreMascota, 20), (int)textBox.y + 15, 20, MAROON);
        EndDrawing();
    }

    ClearBackground(RAYWHITE);

    //Inicializa la mascota
    Mascota* m = (Mascota*)malloc(sizeof(Mascota));
    if (!m) return NULL;

    m->nombre = strdup(nombreMascota);
    m->energia = 100;
    m->monedas = 0;
    m->escenario_actual = escenarioInicial;
    m->inventario = list_create();
    m->sombrero = NULL;
    m->camisa = NULL;
    m->pantalones = NULL;
    m->zapatos = NULL;

    SetMouseCursor(MOUSE_CURSOR_DEFAULT);

    return m;
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





