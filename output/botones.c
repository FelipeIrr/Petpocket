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

//CREAR MASCOTA
Mascota* crearMascota(Escenario* escenarioInicial) {
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

    SetMouseCursor(MOUSE_CURSOR_DEFAULT);

    return m;
}

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
void cambiarEscenario(Mascota* mascota, Array* escenarios) {
    if (mascota->energia < 50) {
        for (int i = 0; i < 90; i++) {
            BeginDrawing();
                ClearBackground(RAYWHITE);
                DrawText("No tienes suficiente energía para cambiar de escenario.", 180, 280, 20, RED);
            EndDrawing();
        }
        return;
    }

    int total = array_size(escenarios);
    int seleccion = -1;

    // Generar botones para cada escenario
    Rectangle botones[total];
    for (int i = 0; i < total; i++) {
        botones[i] = (Rectangle){ 100, 120 + i * 60, 800, 40 };
    }

    while (seleccion == -1 && !WindowShouldClose()) {
        Vector2 mouse = GetMousePosition();

        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText("--- ESCENARIOS DISPONIBLES ---", 300, 40, 25, DARKBLUE);

            for (int i = 0; i < total; i++) {
                Escenario* e = (Escenario*)array_get(escenarios, i);
                DrawRectangleRec(botones[i], LIGHTGRAY);
                DrawRectangleLinesEx(botones[i], 1, GRAY);
                DrawText(TextFormat("%d. %s (Energía: %d, Monedas: %d)", i + 1,
                    e->nombreEscenario, e->req_energia, e->req_monedas),
                    botones[i].x + 10, botones[i].y + 10, 20, BLACK);

                if (CheckCollisionPointRec(mouse, botones[i]) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    seleccion = i;
                }
            }

        EndDrawing();
    }

    if (seleccion < 0 || seleccion >= total) return;

    Escenario* elegido = (Escenario*)array_get(escenarios, seleccion);

    if (mascota->energia >= elegido->req_energia && mascota->monedas >= elegido->req_monedas) {
        mascota->energia -= elegido->req_energia;
        mascota->monedas -= elegido->req_monedas;
        mascota->escenario_actual = elegido;

        for (int i = 0; i < 90; i++) {
            BeginDrawing();
                ClearBackground(RAYWHITE);
                DrawText(TextFormat("Escenario cambiado a: %s", elegido->nombreEscenario), 250, 280, 20, DARKGREEN);
            EndDrawing();
        }

    } else {
        for (int i = 0; i < 90; i++) {
            BeginDrawing();
                ClearBackground(RAYWHITE);
                DrawText("No cumples los requisitos para este escenario.", 220, 280, 20, RED);
            EndDrawing();
        }
    }
}

// ACCESO A TIENDA
void tienda(Mascota* mascota) {
    Map* tienda = mascota->escenario_actual->tienda;
    Pair* pares[100]; // máximo 100 ítems en tienda
    int total = 0;

    // Obtener todos los ítems en la tienda
    for (Pair* par = firstMap(tienda); par != NULL; par = nextMap(tienda)) {
        pares[total++] = par;
    }

    Rectangle botones[total];
    for (int i = 0; i < total; i++) {
        botones[i] = (Rectangle){ 100, 120 + i * 60, 800, 50 };
    }

    int seleccion = -1;

    while (seleccion == -1 && !WindowShouldClose()) {
        Vector2 mouse = GetMousePosition();

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("--- TIENDA ---", 400, 40, 30, DARKBLUE);
        DrawText(TextFormat("Monedas: %d", mascota->monedas), 40, 40, 20, DARKGRAY);

        for (int i = 0; i < total; i++) {
            Item* item = (Item*)pares[i]->value;

            DrawRectangleRec(botones[i], LIGHTGRAY);
            DrawRectangleLinesEx(botones[i], 2, GRAY);
            DrawText(TextFormat("%s (tipo: %s, precio: %d)", item->nombre,
                                item->tipo == COMIDA ? "Comida" : "Aspecto",
                                item->precio),
                     botones[i].x + 10, botones[i].y + 15, 20, BLACK);

            if (CheckCollisionPointRec(mouse, botones[i]) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                seleccion = i;
            }
        }

        EndDrawing();
    }

    if (seleccion < 0 || seleccion >= total) return;

    Item* item = (Item*)pares[seleccion]->value;

    if (mascota->monedas < item->precio) {
        for (int i = 0; i < 90; i++) {
            BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText("No tienes suficientes monedas para comprar este ítem.", 180, 280, 20, RED);
            EndDrawing();
        }
        return;
    }

    mascota->monedas -= item->precio;
    list_pushFront(mascota->inventario, item);
}

//----------------------------------------------------------------------------------------------------------------------------------------
void cargarItemsTienda(Map* tienda) {
    FILE* file = fopen("resources/items.csv", "r");
    
    char line[256];
    fgets(line, sizeof(line), file); // Saltar encabezado

    while (fgets(line, sizeof(line), file)) {
        Item* item = malloc(sizeof(Item));
        char* token = strtok(line, ",");
        item->nombre = strdup(token);

        token = strtok(NULL, ",");
        item->tipo = parseTipo(token);

        token = strtok(NULL, ",");
        item->precio = atoi(token);

        token = strtok(NULL, ",");
        item->valor_energetico = atoi(token);

        token = strtok(NULL, ",\n");
        item->ruta_imagen = token ? strdup(token) : NULL;

        if (item->tipo == ASPECTO && item->ruta_imagen)
            item->aspecto = LoadTexture(item->ruta_imagen);
        else
            item->aspecto = (Texture2D){0};

        insertMap(tienda, item->nombre, item);
    }

    fclose(file);
}

void crearTienda(Escenario* escenario) {
    escenario->tienda = map_create(); //mapa para la tienda del escenario actual

    // Cargar ítems de ejemplo
    cargarItemsTienda(escenario->tienda);
}




