#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"
#include <math.h>
#include "tdas/list.h"
#include "tdas/map.h"
#include "tdas/array.h"

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

void alimentarMascotaRaylib(Mascota* m) {
    Item* item = list_first(m->inventario);  // recorro inventario

    while (item != NULL) {
        if (item->tipo == COMIDA) {
            if (m->energia < 100) {
                m->energia += item->valor_energetico;
                if (m->energia > 100) m->energia = 100;
                list_popCurrent(m->inventario);  // Elimino del inventario la comida utilizada

                // Mensaje de que se alimentó con éxito
                for (int frames = 0; frames < 90; frames++) {
                    BeginDrawing();
                        ClearBackground(RAYWHITE);
                        DrawText("¡La mascota ha comido!", 300, 280, 20, DARKGREEN);
                        DrawText(TextFormat("Energía actual: %d", m->energia), 300, 310, 20, DARKGREEN);
                    EndDrawing();
                }
            } else {
                for (int frames = 0; frames < 90; frames++) {
                    BeginDrawing();
                        ClearBackground(RAYWHITE);
                        DrawText("La energía ya está al máximo.", 300, 290, 20, RED);
                    EndDrawing();
                }
            }
            return;
        }
        item = list_next(m->inventario);
    }

    // No se encontró comida en el inventario 
    for (int frames = 0; frames < 90; frames++) {
        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText("No tienes comida en el inventario.", 250, 290, 20, RED);
        EndDrawing();
    }
}





