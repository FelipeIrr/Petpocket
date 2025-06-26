#include "botones.h"
#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"
#include <math.h>
#include <string.h>
#include <strings.h>
#include "tdas/list.h"
#include "tdas/map.h"
#include "tdas/array.h"

//FUNCIONES AUXILIARES DEL JUEGO
//--------------------------------------------------------------------------------------------------------------------------------------

//ALIMENTAR MASCOTA
void alimentarMascota(Mascota* m) {
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

// AVANZAR DE ESCENARIO
/*void cambiarEscenario(Mascota* mascota, Array* escenarios) {
    if (mascota->energia < 50) {
        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText("No tienes suficiente energía para cambiar de escenario (mínimo 50).", 100, 280, 20, RED);
        EndDrawing();
        return;
    }

    int total = array_size(escenarios);
    int indice = 0;

    while (!WindowShouldClose()) {
        Escenario* e = (Escenario*)array_get(escenarios, indice);

        Rectangle btnAceptar = { 200, 360, 180, 40 };
        Rectangle btnSiguiente = { 500, 360, 180, 40 };

        Vector2 mouse = GetMousePosition();
        bool aceptarPresionado = false;
        bool siguientePresionado = false;

        BeginDrawing();
            ClearBackground(RAYWHITE);

            DrawText("--- CAMBIO DE ESCENARIO ---", 300, 40, 25, DARKBLUE);

            DrawText(TextFormat("Escenario: %s", e->nombreEscenario), 200, 120, 25, BLACK);
            DrawText(TextFormat("Requiere Energía: %d", e->req_energia), 200, 170, 20, DARKGRAY);
            DrawText(TextFormat("Requiere Monedas: %d", e->req_monedas), 200, 200, 20, DARKGRAY);

            // Botón Aceptar
            DrawRectangleRec(btnAceptar, LIGHTGRAY);
            DrawRectangleLinesEx(btnAceptar, 1, GRAY);
            DrawText("Aceptar", btnAceptar.x + 50, btnAceptar.y + 10, 20, BLACK);

            // Botón Siguiente
            DrawRectangleRec(btnSiguiente, LIGHTGRAY);
            DrawRectangleLinesEx(btnSiguiente, 1, GRAY);
            DrawText("Siguiente", btnSiguiente.x + 40, btnSiguiente.y + 10, 20, BLACK);

        EndDrawing();

        // Esperar clic
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (CheckCollisionPointRec(mouse, btnAceptar)) {
                aceptarPresionado = true;
            }
            if (CheckCollisionPointRec(mouse, btnSiguiente)) {
                siguientePresionado = true;
            }
        }

        // Procesar selección
        if (aceptarPresionado) {
            if (mascota->energia >= e->req_energia && mascota->monedas >= e->req_monedas) {
                mascota->energia -= e->req_energia;
                mascota->monedas -= e->req_monedas;

                mascota->escenarioActual = e;

                // Confirmar cambio
                
                    BeginDrawing();
                        ClearBackground(RAYWHITE);
                        DrawText(TextFormat("Escenario cambiado a: %s", e->nombreEscenario), 200, 280, 20, DARKGREEN);
                    EndDrawing();
                
                return; // Salir de la función al cambiar de escenario
            } else {
                
                    BeginDrawing();
                        ClearBackground(RAYWHITE);
                        DrawText("No cumples los requisitos para este escenario.", 220, 280, 20, RED);
                    EndDrawing();
                
            }
        }

        if (siguientePresionado) {
            indice = (indice + 1) % total;
        }
    }
} */



