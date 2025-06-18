#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "tdas/list.h"
#include "tdas/map.h"
#include "tdas/array.h"

 
#define NUM_FRAMES  3       // Number of frames (rectangles) for the button sprite texture
#define NUM_BUTTONS 6
#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 650
// Juego Ritmico
#define NOTE_SIZE 30
#define HIT_ZONE_Y (SCREEN_HEIGHT - 100)
#define HIT_THRESHOLD 0.15f
#define NOTE_SPEED 300.0f

// 1. Enum para los menús
typedef enum {
    MENU_PRINCIPAL,
    MENU_TIENDA,
    MENU_COMIDA,
    MENU_JUEGO,
    MENU_ACCESORIOS,
    MENU_LUGARES,
    MENU_CONFIG
} Pantalla;

typedef struct {
    float time;     // segundo del beat
    bool hit;
} Beat;

Pantalla pantallaActual = MENU_PRINCIPAL;

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

void juegoRitmico() {

    // Asegúrate de que la ventana y el audio estén inicializados antes de llamar a esta función
    // Si ya están inicializados en main, no es necesario volver a inicializarlos aquí

    Music music = LoadMusicStream("resources/Nothing.wav");
    if (!IsMusicValid(music)) {
        // Mostrar mensaje de error en consola y salir de la función
        printf("No se encontro el archivo de audio.\n");
        return;
    }
    PlayMusicStream(music);

    // Definir beatmap (segundos)
    float bpm = 120.0f; // Cambia esto al BPM de tu canción
    float songLength = GetMusicTimeLength(music);
    int beatCount = (int)(songLength * bpm / 60.0f);
    if (beatCount > 512) beatCount = 512; // <-- Limita el número de beats
    Beat beatMap[512]; // Máximo 512 beats

    for (int i = 0; i < beatCount; i++) {
        beatMap[i].time = (60.0f / bpm) * i;
        beatMap[i].hit = false;
    }

    int score = 0;
    char feedbackText[32] = "";
    int feedbackTimer = 0;

    bool exitToMenu = false;
    while (!WindowShouldClose() && !exitToMenu) {
        UpdateMusicStream(music);
        float t = GetMusicTimePlayed(music);

        BeginDrawing();
        ClearBackground(BLACK);

        // Dibujo zona de impacto
        DrawRectangle(0, HIT_ZONE_Y - NOTE_SIZE/2, SCREEN_WIDTH, NOTE_SIZE, GRAY);
        DrawText("PRESS SPACE", SCREEN_WIDTH/2 - 80, HIT_ZONE_Y + 40, 20, RAYWHITE);
        DrawText("BACKSPACE: Volver al menu", 10, SCREEN_HEIGHT - 30, 20, LIGHTGRAY);

        // Dibujar y chequear notas
        for (int i = 0; i < beatCount; i++) {
            if (beatMap[i].hit) continue;

            float dt = t - beatMap[i].time;
            float y = HIT_ZONE_Y - dt * NOTE_SPEED;

            if (y > SCREEN_HEIGHT) continue; // Aún no aparece
            if (y < HIT_ZONE_Y - NOTE_SIZE*2) {
                // Ya pasó la zona -> missed
                beatMap[i].hit = true;
                strcpy(feedbackText, "Miss 😢");
                feedbackTimer = 30;
                continue;
            }
            if (y > -NOTE_SIZE && y < SCREEN_HEIGHT) {
                DrawCircle(SCREEN_WIDTH/2, (int)y, NOTE_SIZE, BLUE);
                if (IsKeyPressed(KEY_SPACE)) {
                    float diff = fabsf(dt);
                    if (diff < HIT_THRESHOLD) {
                        beatMap[i].hit = true;
                        score += (diff < 0.05f ? 300 : 100);
                        strcpy(feedbackText, diff < 0.05f ? "Perfect!" : "Good!");
                        feedbackTimer = 30;
                    }
                }
            }
        }

        // UI: puntaje y feedback
        DrawText(TextFormat("Score: %04i", score), 10, 10, 24, GREEN);
        if (feedbackTimer > 0) {
            DrawText(feedbackText, SCREEN_WIDTH/2 - MeasureText(feedbackText, 24)/2, HIT_ZONE_Y - 60, 24, YELLOW);
            feedbackTimer--;
        }
        
        if (IsKeyPressed(KEY_BACKSPACE)) {
            exitToMenu = true; // Volver al menú
        }

        EndDrawing();
    }

    StopMusicStream(music);
    UnloadMusicStream(music);
}

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1000;
    const int screenHeight = 650;

    InitWindow(screenWidth, screenHeight, "raylib [textures] example - sprite button");

    InitAudioDevice();      // Initialize audio device

    Sound fxButton = LoadSound("resources/buttonfx.wav");   // Load button sound
    Texture2D button = LoadTexture("resources/test.png"); // Load button texture


    // Define frame rectangle for drawing
    float frameHeight = (float)button.height/NUM_FRAMES;
    Rectangle sourceRec = { 0, 0, (float)button.width, frameHeight };

    // Define button bounds on screen
    Rectangle btnBounds[NUM_BUTTONS] = {
        { 50, 500, (float)button.width, frameHeight }, // Botón 1 en (50, 500)
        { 375, 500, (float)button.width, frameHeight }, // Botón 2 en (300, 200)
        { 743, 500, (float)button.width, frameHeight }, // Botón 3 en (500, 300)
        { 50, 376, (float)button.width, frameHeight }, // Botón 4 en (100, 200)
        { 743, 376, (float)button.width, frameHeight }, // Botón 5 en (300, 300)
        { 782, 39, (float)button.width, frameHeight }  // Botón 6 en (500, 100)
    };
    int btnState[NUM_BUTTONS] = { 0 };
    bool btnAction[NUM_BUTTONS] = { false };

    Vector2 mousePoint = { 0.0f, 0.0f };

    // Carga texturas
    Image fondoImg = LoadImage("resources/fondo.png");
    ImageFlipVertical(&fondoImg); // Invierte la imagen verticalmente
    Texture2D fondo = LoadTextureFromImage(fondoImg);
    UnloadImage(fondoImg);
    Texture2D imagen2D = LoadTexture("resources/yipeee!.png"); // Imagen central

    // Crea un plano 3D para el fondo
    Model fondoModel = LoadModelFromMesh(GenMeshPlane(20, 20, 1, 1));
    fondoModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = fondo;

    // Crea un modelo 3D para el ecenario
    Model guitarra = LoadModel("resources/guitarra1.glb");

    // Reemplaza el plano por un skybox (cubo grande)
    Mesh skyboxMesh = GenMeshCube(100.0f, 100.0f, 100.0f); // Cubo grande
    Model skyboxModel = LoadModelFromMesh(skyboxMesh);
    skyboxModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = fondo; // Usa tu imagen panorámica

    // Cámara 3D
    Camera camera = { 0 };
    camera.position = (Vector3){ 0.0f, 2.0f, 0.0f }; // Inicialmente lejos
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    float angle = 0.0f;

    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        mousePoint = GetMousePosition();

        for (int i = 0; i < NUM_BUTTONS; i++) {
            btnAction[i] = false;
            if (CheckCollisionPointRec(mousePoint, btnBounds[i])) {
                if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) btnState[i] = 2;
                else btnState[i] = 1;
                if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) btnAction[i] = true;
            } else btnState[i] = 0;

            if (btnAction[i]) {
                PlaySound(fxButton);
                // Aquí puedes poner una acción diferente para cada botón
            }
        }

        // Actualiza la cámara para girar alrededor del centro
        angle += 0.005f;
        camera.position.x = sinf(angle) * 10.0f;
        camera.position.z = cosf(angle) * 10.0f;
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
            ClearBackground(RAYWHITE);

            // --- Escena 3D SIEMPRE visible ---
            BeginMode3D(camera);
                rlDisableBackfaceCulling();
                DrawModel(skyboxModel, (Vector3){0,0,0}, 1.0f, WHITE);
                rlEnableBackfaceCulling();
                DrawBillboard(camera, imagen2D, (Vector3){0,1,0}, 2.0f, WHITE);
                DrawModel(fondoModel, (Vector3){0,0,0}, 1.0f, WHITE);
                DrawModelEx(guitarra, (Vector3){3,1,0}, (Vector3){-4,4,1}, 45.0f, (Vector3){2,2,2}, WHITE);
            EndMode3D();

            // --- Interfaz 2D según menú ---
            if (pantallaActual == MENU_PRINCIPAL) {
                DrawText("MENU PRINCIPAL", 20, 20, 30, DARKBLUE);
                for (int i = 0; i < NUM_BUTTONS; i++) {
                    sourceRec.y = btnState[i]*frameHeight;
                    DrawTextureRec(button, sourceRec, (Vector2){ btnBounds[i].x, btnBounds[i].y }, WHITE);

                    if (btnAction[i]) {
                        PlaySound(fxButton);
                        if (i == 0) pantallaActual = MENU_COMIDA;        // Botón 1: COMIDA
                        else if (i == 1) pantallaActual = MENU_JUEGO;   // Botón 2: JUEGO
                        else if (i == 2) pantallaActual = MENU_LUGARES; // Botón 3: LUGARES
                        else if (i == 3) pantallaActual = MENU_TIENDA;    // Botón 4: TIENDA
                        else if (i == 4) pantallaActual = MENU_ACCESORIOS;    // Botón 5: ACCESORIOS
                        else if (i == 5) pantallaActual = MENU_CONFIG;   // Botón 6: CONFIGURACION
                    }
                }
            }
            else if (pantallaActual == MENU_JUEGO) {
                DrawText("JUEGO", 20, 20, 30, DARKGREEN);
                DrawText("Presiona Y para jugar", 20, 60, 20, GRAY);
                DrawText("Presiona BACKSPACE para volver", 20, 100, 20, GRAY);
                if (IsKeyPressed(KEY_Y)) {
                    juegoRitmico(); // Llama a la función del juego
                }
                if (IsKeyPressed(KEY_BACKSPACE)) pantallaActual = MENU_PRINCIPAL;
            }
            else if (pantallaActual == MENU_CONFIG) {
                DrawText("CONFIGURACION", 20, 20, 30, DARKGRAY);
                DrawText("Presiona BACKSPACE para volver", 20, 60, 20, GRAY);
                if (IsKeyPressed(KEY_BACKSPACE)) pantallaActual = MENU_PRINCIPAL;
            }
            else if (pantallaActual == MENU_ACCESORIOS) {
                DrawText("ACCESORIOS", 20, 20, 30, DARKPURPLE);
                DrawText("Presiona BACKSPACE para volver", 20, 60, 20, GRAY);
                if (IsKeyPressed(KEY_BACKSPACE)) pantallaActual = MENU_PRINCIPAL;
            }
            else if (pantallaActual == MENU_TIENDA) {
                DrawText("TIENDA", 20, 20, 30, ORANGE);
                DrawText("Presiona BACKSPACE para volver", 20, 60, 20, GRAY);
                if (IsKeyPressed(KEY_BACKSPACE)) pantallaActual = MENU_PRINCIPAL;
            }
            else if (pantallaActual == MENU_COMIDA) {
                DrawText("COMIDA", 20, 20, 30, DARKBROWN);
                DrawText("Presiona BACKSPACE para volver", 20, 60, 20, GRAY);
                if (IsKeyPressed(KEY_BACKSPACE)) pantallaActual = MENU_PRINCIPAL;
            }
            else if (pantallaActual == MENU_LUGARES) {
                DrawText("LUGARES", 20, 20, 30, DARKGRAY);
                DrawText("Presiona BACKSPACE para volver", 20, 60, 20, GRAY);
                if (IsKeyPressed(KEY_BACKSPACE)) pantallaActual = MENU_PRINCIPAL;
            }

            DrawText("a", 10, 10, 20, DARKGRAY);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadTexture(button);  // Unload button texture
    UnloadSound(fxButton);  // Unload sound
    UnloadTexture(fondo);
    UnloadTexture(imagen2D);
    UnloadModel(fondoModel);
    UnloadModel(skyboxModel);

    CloseAudioDevice();     // Close audio device

    CloseWindow();          // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}