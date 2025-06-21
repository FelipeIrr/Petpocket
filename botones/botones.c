#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"
#include <math.h>
#include "tdas/list.h"
#include "tdas/map.h"
#include "tdas/array.h"

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

    for (int i = 0; i < 90; i++) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText(TextFormat("Compraste %s", item->nombre), 300, 280, 20, DARKGREEN);
        DrawText(TextFormat("Monedas restantes: %d", mascota->monedas), 300, 310, 20, DARKGREEN);
        EndDrawing();
    }
}
//----------------------------------------------------------------------------------------------------------------------------------------


//OPCIONES DEL JUEGO
void opcionesDelSistema(Mascota** mascota, Escenario* escenario_inicial) {
    bool cambiarNombre = false;
    bool reiniciar = false;
    bool escribiendo = false;
    char nuevoNombre[50] = "\0";
    int letraPos = 0;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("--- OPCIONES DEL SISTEMA ---", 250, 100, 25, DARKBLUE);
        DrawRectangle(270, 180, 280, 40, LIGHTGRAY);
        DrawRectangle(270, 240, 280, 40, LIGHTGRAY);
        DrawText("1. Cambiar nombre de la mascota", 280, 190, 20, BLACK);
        DrawText("2. Reiniciar partida", 280, 250, 20, BLACK);

        // Si se está escribiendo un nuevo nombre
        if (escribiendo) {
            DrawText("Nuevo nombre:", 300, 320, 20, BLACK);
            DrawRectangle(300, 350, 300, 40, WHITE);
            DrawText(nuevoNombre, 310, 360, 20, BLUE);
        }

        EndDrawing();

        // Manejo de botones por click
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mouse = GetMousePosition();
            if (CheckCollisionPointRec(mouse, (Rectangle){270, 180, 280, 40})) {
                cambiarNombre = true;
                escribiendo = true;
                memset(nuevoNombre, 0, sizeof(nuevoNombre));
                letraPos = 0;
            }
            if (CheckCollisionPointRec(mouse, (Rectangle){270, 240, 280, 40})) {
                reiniciar = true;
            }
        }

        // Captura de texto para nombre
        if (escribiendo) {
            int key = GetCharPressed();
            while (key > 0) {
                if ((key >= 32) && (key <= 125) && letraPos < 49) {
                    nuevoNombre[letraPos++] = (char)key;
                    nuevoNombre[letraPos] = '\0';
                }
                key = GetCharPressed();
            }

            if (IsKeyPressed(KEY_BACKSPACE) && letraPos > 0) {
                letraPos--;
                nuevoNombre[letraPos] = '\0';
            }

            if (IsKeyPressed(KEY_ENTER)) {
                free((*mascota)->nombre);
                (*mascota)->nombre = strdup(nuevoNombre);
                escribiendo = false;

                for (int i = 0; i < 90; i++) {
                    BeginDrawing();
                    ClearBackground(RAYWHITE);
                    DrawText("Nombre cambiado exitosamente.", 250, 280, 20, DARKGREEN);
                    EndDrawing();
                }
                break;
            }
        }

        if (reiniciar) {
            list_clean((*mascota)->inventario);
            free((*mascota)->nombre);
            free(*mascota);

            // Crear nueva mascota con nombre temporal
            strcpy(nuevoNombre, "Nueva Mascota");
            *mascota = crearMascota((*mascota)->escenario_actual);

            for (int i = 0; i < 90; i++) {
                BeginDrawing();
                ClearBackground(RAYWHITE);
                DrawText("Partida reiniciada.", 280, 280, 20, DARKGREEN);
                EndDrawing();
            }
            break;
        }
    }
}


