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


// Las definiciones de TipoItem, Item, Escenario y Mascota ya están en botones.h

//CREAR MASCOTA
Mascota* crearMascota() {
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
    m->inventario = list_create();
    m->aspecto_actual = LoadTexture("resources/yipeee!.png"); // Inicializar sin escenario

    return m;
}

void reiniciar(Mascota** mascota) {
    list_clean((*mascota)->inventario);
    free((*mascota)->nombre);
    free(*mascota);

    // Crear nueva mascota con nombre temporal
    *mascota = crearMascota();
}

void mostrarTienda(Mascota* mascota, Escenario* escenario) {
    Map* tienda = escenario->tienda;
    int total = map_size(tienda);
    if (total == 0) return;

    // Crear arreglo de botones para la selección visual
    Rectangle* botones = malloc(sizeof(Rectangle) * total);
    int seleccion = -1;

    // Recorrer el mapa y mostrar los ítems
    int idx = 0;
    for (Pair* p = firstMap(tienda); p != NULL; p = nextMap(tienda)) {
        botones[idx] = (Rectangle){ 120, 100 + idx * 70, 700, 60 };
        idx++;
    }

    while (seleccion == -1 && !WindowShouldClose()) {
        Vector2 mouse = GetMousePosition();

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("--- TIENDA ---", 400, 40, 30, DARKBLUE);
        DrawText(TextFormat("Monedas: %d", mascota->monedas), 40, 40, 20, DARKGRAY);

        int i = 0;
        for (Pair* p = firstMap(tienda); p != NULL; p = nextMap(tienda)) {
            Item* item = (Item*)p->value;
            DrawRectangleRec(botones[i], LIGHTGRAY);
            DrawRectangleLinesEx(botones[i], 2, GRAY);
            DrawText(TextFormat("%s (precio: %d)", item->nombre, item->precio),
                     botones[i].x + 10, botones[i].y + 15, 20, BLACK);

            if (CheckCollisionPointRec(mouse, botones[i]) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                seleccion = i;
            }
            i++;
        }

        EndDrawing();
    }

    // Selección y compra
    if (seleccion < 0 || seleccion >= total) {
        free(botones);
        return;
    }

    // Buscar el ítem seleccionado
    int i = 0;
    Item* itemSeleccionado = NULL;
    for (Pair* p = firstMap(tienda); p != NULL; p = nextMap(tienda)) {
        if (i == seleccion) {
            itemSeleccionado = (Item*)p->value;
            break;
        }
        i++;
    }

    if (!itemSeleccionado) {
        free(botones);
        return;
    }

    if (mascota->monedas < itemSeleccionado->precio) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("No tienes suficientes monedas para comprar este ítem.", 180, 280, 20, RED);
        EndDrawing();
        free(botones);
        return;
    }

    mascota->monedas -= itemSeleccionado->precio;
    list_pushFront(mascota->inventario, itemSeleccionado);

    free(botones);
}

// Cargar datos del csv
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