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
<<<<<<< HEAD
}

// Función auxiliar para saber si un aspecto ya está en el inventario
bool tieneAspecto(List* inventario, Item* aspecto) {
    for (Item* it = list_first(inventario); it != NULL; it = list_next(inventario)) {
        if (it->tipo == ASPECTO && strcmp(it->nombre, aspecto->nombre) == 0) {
            return true;
        }
    }
    return false;
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

    // Filtrar los ítems que se mostrarán (no mostrar aspectos ya comprados)
    int visibles = 0;
    Item* visiblesItems[100];
    for (int i = 0; i < total; i++) {
        Item* item = (Item*)pares[i]->value;
        if (item->tipo == ASPECTO && tieneAspecto(mascota->inventario, item)) {
            continue; // No mostrar si ya lo tiene
        }
        visiblesItems[visibles++] = item;
    }

    Rectangle botones[visibles];
    for (int i = 0; i < visibles; i++) {
        botones[i] = (Rectangle){ 100, 120 + i * 60, 800, 50 };
    }
=======
>>>>>>> aeeafe2a104f3864b084ff3da42a9621776a095d

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

<<<<<<< HEAD
        for (int i = 0; i < visibles; i++) {
            Item* item = visiblesItems[i];
=======
        // Mostrar contador de ítems cargados
        DrawText(TextFormat("Items cargados: %d", total), 20, 10, 20, DARKGREEN);
>>>>>>> aeeafe2a104f3864b084ff3da42a9621776a095d

        // --- Dibujo con scroll ---
        int i = 0;
        for (i = 0; i < total; i++) {
            // Si es un aspecto y ya tienes al menos uno, no lo muestres
            if (items[i]->tipo == ASPECTO && contadores[i] >= 1) continue;

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

<<<<<<< HEAD
    if (seleccion < 0 || seleccion >= visibles) return;

    Item* item = visiblesItems[seleccion];
=======
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
>>>>>>> aeeafe2a104f3864b084ff3da42a9621776a095d

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

    // Contar cuántos aspectos hay
    int cantidad = 0;
    for (Item* item = list_first(mascota->inventario); item != NULL; item = list_next(mascota->inventario)) {
        if (item->tipo == ASPECTO && item->aspecto.id > 0) cantidad++;
    }
    if (cantidad == 0) {
        bool salir = false;
        while (!salir && !WindowShouldClose()) {
            BeginDrawing();
            ClearBackground(RAYWHITE);
            int anchoTexto = MeasureText("No tienes aspectos disponibles en el inventario.", 20);
            DrawText("No tienes aspectos disponibles en el inventario.", (GetScreenWidth() - anchoTexto) / 2, 300, 20, RED);
            int anchoSalir = MeasureText("Presiona BACKSPACE o ESC para salir", 18);
            DrawText("Presiona BACKSPACE o ESC para salir", (GetScreenWidth() - anchoSalir) / 2, 340, 18, GRAY);
            EndDrawing();
            if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressed(KEY_ESCAPE)) salir = true;
        }
        return;
    }

    // Crear arreglo dinámico de aspectos
    Item** aspectos = malloc(sizeof(Item*) * cantidad);
    int idx = 0;
    for (Item* item = list_first(mascota->inventario); item != NULL; item = list_next(mascota->inventario)) {
        if (item->tipo == ASPECTO && item->aspecto.id > 0) aspectos[idx++] = item;
    }

    int seleccion = 0;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Título
        const char* titulo = "Selecciona un aspecto con <- y ->, ENTER para confirmar";
        int anchoTitulo = MeasureText(titulo, 20);
        DrawText(titulo, (GetScreenWidth() - anchoTitulo) / 2, 50, 20, DARKGRAY);

        // Mostrar todos los aspectos en una fila
        int thumbSize = 100;
        int spacing = 30;
        int totalWidth = cantidad * thumbSize + (cantidad - 1) * spacing;
        int startX = (GetScreenWidth() - totalWidth) / 2;
        int y = 180;

        for (int i = 0; i < cantidad; i++) {
            Texture2D textura = aspectos[i]->aspecto;
            Rectangle dest = { startX + i * (thumbSize + spacing), y, thumbSize, thumbSize };
            // Dibuja marco si está seleccionado
            if (i == seleccion) {
                DrawRectangleLinesEx(dest, 4, GOLD);
            } else {
                DrawRectangleLinesEx(dest, 2, GRAY);
            }
            // Dibuja la textura centrada en el thumbnail
            if (textura.id > 0) {
                float scale = fminf(thumbSize / (float)textura.width, thumbSize / (float)textura.height);
                int drawW = (int)(textura.width * scale);
                int drawH = (int)(textura.height * scale);
                int drawX = dest.x + (thumbSize - drawW) / 2;
                int drawY = dest.y + (thumbSize - drawH) / 2;
                DrawTextureEx(textura, (Vector2){drawX, drawY}, 0, scale, WHITE);
            }
            // Nombre debajo
            int anchoNombre = MeasureText(aspectos[i]->nombre, 18);
            DrawText(aspectos[i]->nombre, dest.x + (thumbSize - anchoNombre) / 2, dest.y + thumbSize + 8, 18, (i == seleccion) ? GOLD : BLACK);
        }

        // Instrucción para salir
        int anchoSalir = MeasureText("BACKSPACE o ESC para salir", 18);
        DrawText("BACKSPACE o ESC para salir", (GetScreenWidth() - anchoSalir) / 2, GetScreenHeight() - 40, 18, GRAY);

        EndDrawing();

        // Navegación
        if (IsKeyPressed(KEY_RIGHT)) {
            if (cantidad > 0) seleccion = (seleccion + 1) % cantidad;
        }
        if (IsKeyPressed(KEY_LEFT)) {
            if (cantidad > 0) seleccion = (seleccion - 1 + cantidad) % cantidad;
        }

        // Confirmar selección
        if (IsKeyPressed(KEY_ENTER)) {
            if (cantidad > 0 && seleccion >= 0 && seleccion < cantidad && aspectos[seleccion]) {
                mascota->aspecto_actual = aspectos[seleccion]->aspecto;

                // Mensaje de confirmación
                bool salirConfirm = false;
                while (!salirConfirm && !WindowShouldClose()) {
                    BeginDrawing();
                    ClearBackground(RAYWHITE);
                    int anchoMsg = MeasureText("¡Aspecto actualizado!", 20);
                    DrawText("¡Aspecto actualizado!", (GetScreenWidth() - anchoMsg) / 2, 300, 20, DARKGREEN);
                    int anchoSalir = MeasureText("Presiona BACKSPACE o ESC para salir", 18);
                    DrawText("Presiona BACKSPACE o ESC para salir", (GetScreenWidth() - anchoSalir) / 2, 340, 18, GRAY);
                    EndDrawing();

                    if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_ENTER))
                        salirConfirm = true;
                }
            }
            break;
        }

        // Salir sin cambiar
        if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE)) break;
    }

    free(aspectos);
}

void alimentarMascota(Mascota* mascota) {
    if (list_size(mascota->inventario) == 0) {
        bool salir = false;
        while (!salir && !WindowShouldClose()) {
            BeginDrawing();
                ClearBackground(RAYWHITE);
                DrawText("No tienes comida en el inventario.", 250, 280, 20, RED);

                int anchoSalir = MeasureText("Presiona BACKSPACE o ESC para salir", 18);
                DrawText("Presiona BACKSPACE o ESC para salir", (GetScreenWidth() - anchoSalir) / 2, 400, 18, GRAY);
            EndDrawing();

            if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressed(KEY_ESCAPE)) {
                salir = true;
            }
        }
        return;
    }

    // --- Contar y agrupar comidas únicas ---
    // Primero, contar cuántos tipos únicos de comida hay
    typedef struct {
        Item* item;
        int cantidad;
    } ComidaContador;

    int total = 0;
    // Máximo: todos los items del inventario son comida distinta
    ComidaContador* comidas = malloc(sizeof(ComidaContador) * list_size(mascota->inventario));

    for (Item* item = list_first(mascota->inventario); item != NULL; item = list_next(mascota->inventario)) {
        if (item->tipo != COMIDA) continue;
        // Buscar si ya está en el arreglo
        int encontrado = 0;
        for (int i = 0; i < total; i++) {
            if (strcmp(comidas[i].item->nombre, item->nombre) == 0) {
                comidas[i].cantidad++;
                encontrado = 1;
                break;
            }
        }
        if (!encontrado) {
            comidas[total].item = item;
            comidas[total].cantidad = 1;
            total++;
        }
    }

    if (total == 0) {
        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText("No tienes comida en el inventario.", 250, 280, 20, RED);
        EndDrawing();
        free(comidas);
        return;
    }

    int seleccion = 0;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Título
        const char* titulo = "Selecciona una comida con <- y ->, ENTER para alimentar";
        int anchoTitulo = MeasureText(titulo, 20);
        DrawText(titulo, (GetScreenWidth() - anchoTitulo) / 2, 50, 20, DARKGRAY);

        // Mostrar energía actual
        char energiaTxt[64];
        snprintf(energiaTxt, sizeof(energiaTxt), "Energía actual: %d / 100", mascota->energia);
        int anchoEnergia = MeasureText(energiaTxt, 20);
        DrawText(energiaTxt, (GetScreenWidth() - anchoEnergia) / 2, 90, 20, DARKBLUE);

        // Mostrar todas las comidas en una fila
        int thumbSize = 100;
        int spacing = 30;
        int totalWidth = total * thumbSize + (total - 1) * spacing;
        int startX = (GetScreenWidth() - totalWidth) / 2;
        int y = 180;

        for (int i = 0; i < total; i++) {
            Texture2D textura = comidas[i].item->aspecto;
            Rectangle dest = { startX + i * (thumbSize + spacing), y, thumbSize, thumbSize };
            // Dibuja marco si está seleccionado
            if (i == seleccion) {
                DrawRectangleLinesEx(dest, 4, GOLD);
            } else {
                DrawRectangleLinesEx(dest, 2, GRAY);
            }
            // Dibuja la textura centrada en el thumbnail
            if (textura.id > 0) {
                float scale = fminf(thumbSize / (float)textura.width, thumbSize / (float)textura.height);
                int drawW = (int)(textura.width * scale);
                int drawH = (int)(textura.height * scale);
                int drawX = dest.x + (thumbSize - drawW) / 2;
                int drawY = dest.y + (thumbSize - drawH) / 2;
                DrawTextureEx(textura, (Vector2){drawX, drawY}, 0, scale, WHITE);
            }
            // Nombre debajo
            int anchoNombre = MeasureText(comidas[i].item->nombre, 18);
            DrawText(comidas[i].item->nombre, dest.x + (thumbSize - anchoNombre) / 2, dest.y + thumbSize + 8, 18, (i == seleccion) ? GOLD : BLACK);

            // Cantidad debajo del nombre
            char cantidadTxt[32];
            snprintf(cantidadTxt, sizeof(cantidadTxt), "x%d", comidas[i].cantidad);
            int anchoCantidad = MeasureText(cantidadTxt, 16);
            DrawText(cantidadTxt, dest.x + (thumbSize - anchoCantidad) / 2, dest.y + thumbSize + 30, 16, DARKGREEN);

            // Valor energético debajo de la cantidad
            char valorEnergeticoTxt[32];
            snprintf(valorEnergeticoTxt, sizeof(valorEnergeticoTxt), "+%d energía", comidas[i].item->valor_energetico);
            int anchoValor = MeasureText(valorEnergeticoTxt, 16);
            DrawText(valorEnergeticoTxt, dest.x + (thumbSize - anchoValor) / 2, dest.y + thumbSize + 50, 16, BLUE);
        }

        // Instrucción para salir
        int anchoSalir = MeasureText("BACKSPACE o ESC para salir", 18);
        DrawText("BACKSPACE o ESC para salir", (GetScreenWidth() - anchoSalir) / 2, GetScreenHeight() - 40, 18, GRAY);

        EndDrawing();

        // Navegación
        if (IsKeyPressed(KEY_RIGHT)) {
            if (total > 0) seleccion = (seleccion + 1) % total;
        }
        if (IsKeyPressed(KEY_LEFT)) {
            if (total > 0) seleccion = (seleccion - 1 + total) % total;
        }

        // Confirmar selección
        if (IsKeyPressed(KEY_ENTER)) {
            if (total > 0 && seleccion >= 0 && seleccion < total && comidas[seleccion].cantidad > 0) {
                // Si la energía ya está al máximo, no gastar comida ni sumar energía
                if (mascota->energia >= 100) {
                    // Mensaje: energía al máximo
                    bool salirConfirm = false;
                    while (!salirConfirm && !WindowShouldClose()) {
                        BeginDrawing();
                        ClearBackground(RAYWHITE);
                        int anchoMsg = MeasureText("¡La energía ya está al máximo!", 20);
                        DrawText("¡La energía ya está al máximo!", (GetScreenWidth() - anchoMsg) / 2, 300, 20, RED);
                        int anchoSalir = MeasureText("Presiona BACKSPACE o ESC para salir", 18);
                        DrawText("Presiona BACKSPACE o ESC para salir", (GetScreenWidth() - anchoSalir) / 2, 340, 18, GRAY);
                        EndDrawing();

                        if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_ENTER))
                            salirConfirm = true;
                    }
                    break;
                }

                // Alimentar: sumar energía y eliminar un item de ese tipo del inventario
                mascota->energia += comidas[seleccion].item->valor_energetico;
                if (mascota->energia > 100) mascota->energia = 100;

                // Eliminar un item de ese tipo del inventario
                for (Item* item = list_first(mascota->inventario); item != NULL; item = list_next(mascota->inventario)) {
                    if (item->tipo == COMIDA && strcmp(item->nombre, comidas[seleccion].item->nombre) == 0) {
                        list_popCurrent(mascota->inventario);
                        break;
                    }
                }

                // Mensaje de confirmación
                bool salirConfirm = false;
                while (!salirConfirm && !WindowShouldClose()) {
                    BeginDrawing();
                    ClearBackground(RAYWHITE);
                    int anchoMsg = MeasureText("¡Mascota alimentada!", 20);
                    DrawText("¡Mascota alimentada!", (GetScreenWidth() - anchoMsg) / 2, 300, 20, DARKGREEN);
                    int anchoSalir = MeasureText("Presiona BACKSPACE o ESC para salir", 18);
                    DrawText("Presiona BACKSPACE o ESC para salir", (GetScreenWidth() - anchoSalir) / 2, 340, 18, GRAY);
                    EndDrawing();

                    if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_ENTER))
                        salirConfirm = true;
                }
            }
            break;
        }

        // Salir sin alimentar
        if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE)) break;
    }

    free(comidas);
}
