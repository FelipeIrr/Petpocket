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

    // --- Crear arreglo de punteros a ítems para ordenar ---
    Item** items = malloc(sizeof(Item*) * total);
    int idx = 0;
    for (Pair* p = firstMap(tienda); p != NULL; p = nextMap(tienda)) {
        items[idx++] = (Item*)p->value;
    }

    // --- Ordenar de menor a mayor precio ---
    for (int i = 0; i < total - 1; i++) {
        for (int j = i + 1; j < total; j++) {
            if (items[i]->precio > items[j]->precio) {
                Item* tmp = items[i];
                items[i] = items[j];
                items[j] = tmp;
            }
        }
    }

    // Crear arreglo de botones para la selección visual
    Rectangle* botones = malloc(sizeof(Rectangle) * total);
    int seleccion = -1;
    bool salir = false;

    // Crear arreglo para contar cuántos de cada item tiene la mascota
    int* contadores = calloc(total, sizeof(int));

    // Llenar los contadores revisando el inventario de la mascota
    for (int i = 0; i < total; i++) {
        int cuenta = 0;
        for (Item* itemInv = list_first(mascota->inventario); itemInv != NULL; itemInv = list_next(mascota->inventario)) {
            if (strcmp(items[i]->nombre, itemInv->nombre) == 0) cuenta++;
        }
        contadores[i] = cuenta;
        botones[i] = (Rectangle){ 120, 100 + i * 70, 700, 60 };
    }

    // --- Scroll variables ---
    int scrollY = 0;
    int maxScroll = (total * 70) - (GetScreenHeight() - 120);
    if (maxScroll < 0) maxScroll = 0;

    while (!WindowShouldClose() && !salir && seleccion == -1) {
        Vector2 mouse = GetMousePosition();

        // --- Scroll con rueda del mouse ---
        int wheel = GetMouseWheelMove();
        scrollY -= wheel * 30;
        if (scrollY < 0) scrollY = 0;
        if (scrollY > maxScroll) scrollY = maxScroll;

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("--- TIENDA ---", 400, 40, 30, DARKBLUE);
        DrawText(TextFormat("Monedas: %d", mascota->monedas), 40, 40, 20, DARKGRAY);

        // Mostrar contador de ítems cargados
        DrawText(TextFormat("Items cargados: %d", total), 20, 10, 20, DARKGREEN);

        // --- Dibujo con scroll ---
        int i = 0;
        for (i ; i < total; i++) {
            Rectangle btn = botones[i];
            btn.y -= scrollY;
            if (btn.y + btn.height < 80 || btn.y > GetScreenHeight() - 40) continue; // Fuera de la vista

            Item* item = items[i];
            DrawRectangleRec(btn, LIGHTGRAY);
            DrawRectangleLinesEx(btn, 2, GRAY);
            DrawText(TextFormat("%s (precio: %d)", item->nombre, item->precio),
                     btn.x + 10, btn.y + 10, 20, BLACK);
            if (item->tipo == COMIDA) {
                DrawText("Tipo: Comida", btn.x + 10, btn.y + 35, 18, DARKGREEN);
                DrawText(TextFormat("Valor energético: %d", item->valor_energetico), btn.x + 200, btn.y + 35, 18, DARKGREEN);
            } else if (item->tipo == ASPECTO) {
                DrawText("Tipo: Aspecto", btn.x + 10, btn.y + 35, 18, DARKBLUE);
                // Mostrar imagen más pequeña
                if (item->ruta_imagen && strlen(item->ruta_imagen) > 0 && item->aspecto.id > 0) {
                    float scale = 0.25f; // Escala más pequeña
                    float imgH = item->aspecto.height * scale;
                    float posX = btn.x + 300;
                    float posY = btn.y + (btn.height - imgH) / 2;
                    DrawTextureEx(item->aspecto, (Vector2){posX, posY}, 0, scale, WHITE);
                }
            }
            DrawText(TextFormat("Cantidad: %d", contadores[i]), btn.x + 600, btn.y + 20, 18, MAROON);

            // Selección de ítem (solo cuando se suelta el mouse sobre el botón)
            if (CheckCollisionPointRec(mouse, btn) && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                seleccion = i;
            }
        }

        // Mostrar texto de salir en la esquina derecha
        int anchoTexto = MeasureText("Backspace para salir de la tienda", 18);
        DrawText("Backspace para salir de la tienda", GetScreenWidth() - anchoTexto - 20, 20, 18, GRAY);

        EndDrawing();

        // Salir con BACKSPACE, solo si mouseReleased
        if (IsKeyPressed(KEY_BACKSPACE)) {
            salir = true;
        }
    }

    // Si se salió sin seleccionar, limpiar y continuar (no return)
    if (seleccion < 0 || seleccion >= total || salir) {
        free(botones);
        free(contadores);
        free(items);
        return;
    }

    // Buscar el ítem seleccionado
    Item* itemSeleccionado = items[seleccion];

    if (!itemSeleccionado) {
        free(botones);
        free(contadores);
        free(items);
        return;
    }

    if (mascota->monedas < itemSeleccionado->precio) {
        bool salirMensaje = false;
        Rectangle btnOk = { 400, 350, 120, 40 };

        // Variable para controlar si el mouse fue soltado antes de mostrar el mensaje
        // Esperar a que el mouse esté soltado antes de mostrar el mensaje
        while (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && !WindowShouldClose()) {
            // Espera activa para evitar interacción accidental
        }

        while (!salirMensaje && !WindowShouldClose()) {
            Vector2 mouse = GetMousePosition();

            BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText("No tienes suficientes monedas para comprar este ítem.", 180, 280, 20, RED);

            // Botón OK
            bool hoverOk = CheckCollisionPointRec(mouse, btnOk);
            DrawRectangleRec(btnOk, hoverOk ? SKYBLUE : LIGHTGRAY);
            DrawRectangleLinesEx(btnOk, 2, GRAY);
            DrawText("OK", btnOk.x + 35, btnOk.y + 10, 20, BLACK);

            // Mostrar texto de salir en la esquina derecha
            int anchoTexto = MeasureText("Backspace para salir de la tienda", 18);
            DrawText("Backspace para salir de la tienda", GetScreenWidth() - anchoTexto - 20, 20, 18, GRAY);

            EndDrawing();

            // Permitir cerrar con click en OK, ENTER, ESCAPE o BACKSPACE
            if ((hoverOk && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) ||
                IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE)) {
                salirMensaje = true;
            }
        }
        // Esperar a que el mouse se suelte antes de continuar para evitar clicks accidentales
        while (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && !WindowShouldClose()) {
            // Espera activa
        }
        free(botones);
        free(contadores);
        free(items);
        return;
    }

    mascota->monedas -= itemSeleccionado->precio;
    list_pushFront(mascota->inventario, itemSeleccionado);

    free(botones);
    free(contadores);
    free(items);
    return;
}

void cargarItemsTienda(Map* tienda) {
    char* texto = LoadFileText("resources/items.csv");
    if (!texto) return;

    char* linea;
    int linea_num = 0;

    char* saveptr;
    linea = strtok_r(texto, "\r\n", &saveptr);
    while (linea) {
        if (strlen(linea) == 0) {
            linea = strtok_r(NULL, "\r\n", &saveptr);
            continue; // Salta líneas vacías
        }

        // Saltar la primera línea (primer dato)
        if (linea_num == 0) {
            linea_num++;
            linea = strtok_r(NULL, "\r\n", &saveptr);
            continue;
        }

        char buffer[512];
        strncpy(buffer, linea, sizeof(buffer) - 1);
        buffer[sizeof(buffer) - 1] = '\0';

        // Tokenizar los campos
        char* nombre = strtok(buffer, ",");
        char* tipo = strtok(NULL, ",");
        char* precioStr = strtok(NULL, ",");
        char* valorStr = strtok(NULL, ",");
        char* ruta = strtok(NULL, ",");

        if (!nombre || !tipo || !precioStr || !valorStr) {
            linea = strtok_r(NULL, "\r\n", &saveptr);
            continue; // Línea mal formada
        }

        Item* item = malloc(sizeof(Item));
        item->nombre = strdup(nombre);
        item->tipo = (strcmp(tipo, "COMIDA") == 0) ? COMIDA : ASPECTO;
        item->precio = atoi(precioStr);
        item->valor_energetico = atoi(valorStr);

        if (item->tipo == ASPECTO && ruta && strlen(ruta) > 0) {
            item->ruta_imagen = strdup(ruta);
            item->aspecto = LoadTexture(item->ruta_imagen);
        } else {
            item->ruta_imagen = NULL;
            item->aspecto = (Texture2D){ 0 };
        }

        insertMap(tienda, item->nombre, item);
        linea_num++;
        linea = strtok_r(NULL, "\r\n", &saveptr);
    }

    UnloadFileText(texto);
}

void crearTienda(Escenario* escenario) {
    escenario->tienda = createMap(100);
    cargarItemsTienda(escenario->tienda);
} 

Array* cargarEscenarios() {
    Array* escenarios = array_create(7);
    char* texto = LoadFileText("resources/escenarios.csv");
    if (!texto) return escenarios;

    char* saveptr_linea;
    char* linea = strtok_r(texto, "\r\n", &saveptr_linea);
    int linea_num = 0;

    while (linea) {
        if (strlen(linea) == 0) {
            linea = strtok_r(NULL, "\r\n", &saveptr_linea);
            continue; // Saltar líneas vacías
        }

        // Saltar encabezado
        if (linea_num == 0) {
            linea_num++;
            linea = strtok_r(NULL, "\r\n", &saveptr_linea);
            continue;
        }

        // Reservar escenario
        Escenario* escenario = malloc(sizeof(Escenario));
        escenario->nombreEscenario = NULL;
        escenario->imagen_fondo = (Texture2D){ 0 };
        escenario->tienda = NULL;

        // Copiar la línea a un buffer temporal para tokenizar
        char buffer[512];
        strncpy(buffer, linea, sizeof(buffer) - 1);
        buffer[sizeof(buffer) - 1] = '\0';

        // Tokenizar los campos
        char* saveptr_campo;
        char* nombre = strtok_r(buffer, ",", &saveptr_campo);
        char* energiaStr = strtok_r(NULL, ",", &saveptr_campo);
        char* monedasStr = strtok_r(NULL, ",", &saveptr_campo);
        char* rutaFondo = strtok_r(NULL, ",", &saveptr_campo);

        if (!nombre || !energiaStr || !monedasStr) {
            free(escenario);
            linea = strtok_r(NULL, "\r\n", &saveptr_linea);
            continue; // Línea mal formada
        }

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

        linea_num++;
        linea = strtok_r(NULL, "\r\n", &saveptr_linea);
    }
    UnloadFileText(texto);
    return escenarios;
}

void cambiarEscenario(Mascota* mascota, Array* escenarios) {
    if (mascota->energia < 50) {
        bool salir = false;
        Rectangle btnSalir = { 400, 350, 120, 40 };
        while (!WindowShouldClose() && !salir) {
            Vector2 mouse = GetMousePosition();
            BeginDrawing();
                ClearBackground(RAYWHITE);
                DrawText("No tienes suficiente energía para cambiar de escenario (mínimo 50).", 100, 280, 20, RED);

                // Botón Salir
                bool hoverSalir = CheckCollisionPointRec(mouse, btnSalir);
                DrawRectangleRec(btnSalir, hoverSalir ? SKYBLUE : LIGHTGRAY);
                DrawRectangleLinesEx(btnSalir, 2, GRAY);
                DrawText("Salir", btnSalir.x + 30, btnSalir.y + 10, 20, BLACK);

                int anchoTexto = MeasureText("Backspace para salir", 18);
                DrawText("Backspace para salir", GetScreenWidth() - anchoTexto - 20, 20, 18, GRAY);
            EndDrawing();

            if ((hoverSalir && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) || IsKeyPressed(KEY_BACKSPACE)) {
                salir = true;
            }
        }
        return;
    }

    int total = array_size(escenarios);
    // Buscar el índice del escenario actual
    int indice = 0;
    for (int i = 0; i < total; i++) {
        if (mascota->escenarioActual == array_get(escenarios, i)) {
            indice = i;
            break;
        }
    }

    // Solo permitir avanzar al siguiente escenario si no es el último
    if (indice >= total - 1) {
        bool salir = false;
        Rectangle btnSalir = { 400, 350, 120, 40 };
        while (!WindowShouldClose() && !salir) {
            Vector2 mouse = GetMousePosition();
            BeginDrawing();
                ClearBackground(RAYWHITE);
                DrawText("Ya estás en el último escenario disponible.", 200, 280, 20, DARKGRAY);

                // Botón Salir
                bool hoverSalir = CheckCollisionPointRec(mouse, btnSalir);
                DrawRectangleRec(btnSalir, hoverSalir ? SKYBLUE : LIGHTGRAY);
                DrawRectangleLinesEx(btnSalir, 2, GRAY);
                DrawText("Salir", btnSalir.x + 30, btnSalir.y + 10, 20, BLACK);

                int anchoTexto = MeasureText("Backspace para salir", 18);
                DrawText("Backspace para salir", GetScreenWidth() - anchoTexto - 20, 20, 18, GRAY);
            EndDrawing();

            if ((hoverSalir && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) || IsKeyPressed(KEY_BACKSPACE)) {
                salir = true;
            }
        }
        return;
    }

    Escenario* e = (Escenario*)array_get(escenarios, indice + 1);

    Rectangle btnAceptar = { 200, 360, 180, 40 };
    Vector2 mouse;
    bool aceptarPresionado = false;
    bool salir = false;

    while (!WindowShouldClose() && !salir) {
        mouse = GetMousePosition();

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

            // Solo mostrar "Siguiente ->" si hay más escenarios
            if (indice + 1 < total - 1) {
                DrawText("Siguiente ->", 650, 250, 20, GRAY);
            }
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

                // Mostrar mensaje por 2 segundos
                double start = GetTime();
                while (GetTime() - start < 2.0 && !WindowShouldClose()) {
                    BeginDrawing();
                        ClearBackground(RAYWHITE);
                        DrawText(TextFormat("Escenario cambiado a: %s", e->nombreEscenario), 200, 280, 20, Fade(DARKGREEN, 0.5f));
                    EndDrawing();
                }
                salir = true;
            } else {
                // Mostrar mensaje de requisitos insuficientes con botón y backspace
                bool salirMsg = false;
                Rectangle btnSalir = { 400, 350, 120, 40 };
                while (!WindowShouldClose() && !salirMsg) {
                    Vector2 mouseMsg = GetMousePosition();
                    BeginDrawing();
                        ClearBackground(RAYWHITE);
                        DrawText("No cumples los requisitos para este escenario (monedas y/o energía insuficientes).", 220, 280, 20, RED);

                        // Botón Salir
                        bool hoverSalir = CheckCollisionPointRec(mouseMsg, btnSalir);
                        DrawRectangleRec(btnSalir, hoverSalir ? SKYBLUE : LIGHTGRAY);
                        DrawRectangleLinesEx(btnSalir, 2, GRAY);
                        DrawText("Salir", btnSalir.x + 30, btnSalir.y + 10, 20, BLACK);

                        int anchoTexto = MeasureText("Backspace para salir", 18);
                        DrawText("Backspace para salir", GetScreenWidth() - anchoTexto - 20, 20, 18, GRAY);
                    EndDrawing();

                    if ((hoverSalir && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) || IsKeyPressed(KEY_BACKSPACE)) {
                        salirMsg = true;
                    }
                }
                salir = true;
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
