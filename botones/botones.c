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

// Declaración de la función parseTipo
TipoItem parseTipo(const char* tipoStr);

// Las definiciones de TipoItem, Item, Escenario y Mascota ya están en botones.h

Mascota* crearMascota() { //Escenario* escenario) 
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
    //m->escenario_actual =  //TESTING

    return m;
}

void reiniciar(Mascota** mascota) {
    list_clean((*mascota)->inventario);
    free((*mascota)->nombre);
    free(*mascota);

    // Crear nueva mascota con nombre temporal
    *mascota = crearMascota(); //agregar escenario
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
    return;
} 

void cargarItemsTienda(Map* tienda) {
    char* texto = LoadFileText("resources/items.csv");
    
    char* linea = strtok(texto, "\r\n"); // Saltar encabezado
    linea = strtok(NULL, "\r\n");

    while (linea != NULL) {
        // Reservar ítem
        Item* item = malloc(sizeof(Item));
        item->ruta_imagen = NULL;
        item->nombre = NULL;

        // Tokenizar los campos
        char* nombre = strtok(linea, ",");
        char* tipo = strtok(NULL, ",");
        char* precioStr = strtok(NULL, ",");
        char* valorStr = strtok(NULL, ",");
        char* ruta = strtok(NULL, ",");

        // Copiar datos
        item->nombre = strdup(nombre);
        item->tipo = (strcmp(tipo, "COMIDA") == 0) ? COMIDA : ASPECTO;
        item->precio = atoi(precioStr);
        item->valor_energetico = atoi(valorStr);

        // Si es aspecto, cargar textura y guardar ruta
        if (item->tipo == ASPECTO && ruta && strlen(ruta) > 0) {
            item->ruta_imagen = strdup(ruta);
            item->aspecto = LoadTexture(item->ruta_imagen);
        } else {
            item->aspecto = (Texture2D){ 0 };
        }

        // Insertar al mapa con clave el nombre
        insertMap(tienda, item->nombre, item);

        // Siguiente línea
        linea = strtok(NULL, "\r\n");
    }

    UnloadFileText(texto);
}

void crearTienda(Escenario* escenario) {
    escenario->tienda = createMap(100);
    cargarItemsTienda(escenario->tienda);
} 

Array* cargarEscenarios(){
    Array* escenarios = array_create(7);
    char* texto = LoadFileText("resources/escenarios.csv");
    char* linea = strtok(texto, "\r\n"); // Saltar encabezado   
    linea = strtok(NULL, "\r\n");
    while (linea != NULL) {
        // Reservar escenario
        Escenario* escenario = malloc(sizeof(Escenario));
        escenario->nombreEscenario = NULL;
        escenario->imagen_fondo = (Texture2D){ 0 };
        escenario->tienda = NULL;

        // Tokenizar los campos
        char* nombre = strtok(linea, ",");
        char* energiaStr = strtok(NULL, ",");
        char* monedasStr = strtok(NULL, ",");
        char* rutaFondo = strtok(NULL, ",");

        // Copiar datos
        escenario->nombreEscenario = strdup(nombre);
        escenario->req_energia = atoi(energiaStr);
        escenario->req_monedas = atoi(monedasStr);
        
        // Cargar imagen de fondo
        if (rutaFondo && strlen(rutaFondo) > 0) {
            Image img = LoadImage(rutaFondo);
            ImageFlipVertical(&img); // Invertir imagen verticalmente
            escenario->imagen_fondo = LoadTextureFromImage(img);
            UnloadImage(img);
        }

        // Insertar al array de escenarios
        array_push_back(escenarios, escenario);

        // Siguiente línea
        linea = strtok(NULL, "\r\n");
    }
    UnloadFileText(texto);
    return escenarios;
}

void cambiarEscenario(Mascota* mascota, Array* escenarios) {
    if (mascota->energia < 50) {
        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText("No tienes suficiente energía para cambiar de escenario (mínimo 50).", 100, 280, 20, RED);
        EndDrawing();
    }

    int total = array_size(escenarios);
    int indice = 0;

    while (!WindowShouldClose()) {
        Escenario* e = (Escenario*)array_get(escenarios, indice);

        Rectangle btnAceptar = { 200, 360, 180, 40 };
        Vector2 mouse = GetMousePosition();
        bool aceptarPresionado = false;

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

        EndDrawing();

        // Salir si se presiona BACKSPACE
        if (IsKeyPressed(KEY_BACKSPACE)) {
            break;
        }

        // Esperar clic
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (CheckCollisionPointRec(mouse, btnAceptar)) {
                aceptarPresionado = true;
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
                    DrawText("No cumples los requisitos para este escenario(monedas y/o energía insufucuentes).", 220, 280, 20, RED);
                EndDrawing();
            }
        }
    }
}

void cambiarAspectoMascota(Mascota* mascota) {
    int cantidad = 0;
    Item* aspectos[20];  //  20 aspectos distintos (recordar ampliar esto)

    void* actual = list_first(mascota->inventario);
    while (actual && cantidad < 20) {
        Item* item = (Item*)actual;
        if (item->tipo == ASPECTO) {
            aspectos[cantidad++] = item;
        }
        actual = list_next(mascota->inventario);
    }

    if (cantidad == 0) { // No hay aspectos disponibles
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("No tienes aspectos disponibles en el inventario.", 300, 300, 20, RED);
        EndDrawing();
        return;
    }

    int seleccion = 0;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Selecciona un aspecto con <- y ->, ENTER para confirmar", 180, 50, 20, DARKGRAY);

        // Dibuja el aspecto seleccionado
        DrawTexture(aspectos[seleccion]->aspecto, 400, 150, WHITE);
        DrawText(aspectos[seleccion]->nombre, 400, 400, 20, BLACK);

        EndDrawing();

        // Confirmar selección
        if (IsKeyPressed(KEY_ENTER)) {
            mascota->aspecto_actual = aspectos[seleccion]->aspecto;
            BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText("¡Aspecto actualizado!", 300, 300, 20, DARKGREEN);
            EndDrawing();
            break;
        }

        // Salir sin cambiar si presiona ESC
        if (IsKeyPressed(KEY_ESCAPE)) break;
    }
}
