#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "botones/juego.h"
#include "botones/botones.h"
#include "tdas/list.h"
#include "tdas/map.h"
#include "tdas/array.h"

#define NUM_FRAMES  3       // Number of frames (rectangles) for the button sprite texture
#define NUM_BUTTONS 6

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

Pantalla pantallaActual = MENU_PRINCIPAL;

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1000;
    const int screenHeight = 650;

    InitWindow(screenWidth, screenHeight, "PetPocket Mascota Personal");

    InitAudioDevice();      // Initialize audio device
    SetMasterVolume(0.5f);  // Set master volume to 50%

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

    // Carga texturas //Puedo usar fondo como imagen predeterminada para crear mascota, pero debo hacerla tipo escenario
    Image fondoImg = LoadImage("resources/fondo.png");
    ImageFlipVertical(&fondoImg); // Invierte la imagen verticalmente
    Texture2D fondo = LoadTextureFromImage(fondoImg);
    UnloadImage(fondoImg);

    // Crea un plano 3D para el fondo
    Model fondoModel = LoadModelFromMesh(GenMeshPlane(20, 30, 1, 1));
    fondoModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = fondo;
    
    // Cargar textura para el piso
    Texture2D pisoTexture = LoadTexture("resources/piso.png");
    Mesh pisoMesh = GenMeshPlane(20, 20, 1, 1);
    Model Piso = LoadModelFromMesh(pisoMesh);
    Piso.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = pisoTexture;

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

    // Ejemplo: 3 texturas diferentes para los botones
    Texture2D buttonTextures[NUM_BUTTONS];
    buttonTextures[0] = LoadTexture("resources/botonComida.png");        //boton comida
    buttonTextures[1] = LoadTexture("resources/botonJuego.png");         //boton juego
    buttonTextures[2] = LoadTexture("resources/botonLugares.png");       //boton lugares
    buttonTextures[3] = LoadTexture("resources/botonTienda.png");        //boton tienda
    buttonTextures[4] = LoadTexture("resources/botonInventario.png");    //boton accesorios
    buttonTextures[5] = LoadTexture("resources/botonConfig.png");        //boton configuracion

    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------
    Array* vectorEscenarios = cargarEscenarios(); 
    Mascota* Pocket = crearMascota();
    Pocket->escenarioActual = array_get(vectorEscenarios, 0); // Asignar el primer escenario al Pocket al inicializar 
    crearTienda(array_get(vectorEscenarios, 0)); 
    
    // Main game loop
    while (!WindowShouldClose())    // Main game loop
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

        // --- Actualiza la textura del fondo según el escenario actual ---
        skyboxModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = Pocket->escenarioActual->imagen_fondo;

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
                DrawBillboard(camera, Pocket->aspecto_actual, (Vector3){0,0.8,0}, 2.0f, WHITE);
                DrawModel(Piso, (Vector3){0,0,0}, 1.0f, WHITE);
                DrawModelEx(guitarra, (Vector3){3,1,0}, (Vector3){-4,4,1}, 45.0f, (Vector3){2,2,2}, WHITE);
            EndMode3D();

            // --- Interfaz 2D según menú ---
            if (pantallaActual == MENU_PRINCIPAL) {
                DrawText("MENU PRINCIPAL", 20, 20, 30, DARKBLUE);
                DrawText(TextFormat("Monedas: %d", Pocket->monedas), 20, 60, 20, GOLD);
                DrawText(TextFormat("Energia: %d/100", Pocket->energia), 20, 90, 20, GOLD);
                DrawText(TextFormat("Nombre: %s", Pocket->nombre), 20, 120, 20, GOLD);
                // Dibuja un marco semitransparente detrás de los textos de monedas, energía y nombre
                DrawRectangle(10, 10, 320, 140, Fade(BLACK, 0.2f)); // Marco/fondo para info
                for (int i = 0; i < NUM_BUTTONS; i++) {
                    sourceRec.y = btnState[i]*frameHeight;
                    DrawTextureRec(buttonTextures[i], sourceRec, (Vector2){ btnBounds[i].x, btnBounds[i].y }, WHITE);

                    if (btnAction[i]) {
                        PlaySound(fxButton);
                        if (i == 0) pantallaActual = MENU_COMIDA;           // Botón 1: COMIDA
                        else if (i == 1) pantallaActual = MENU_JUEGO;       // Botón 2: JUEGO
                        else if (i == 2) pantallaActual = MENU_LUGARES;     // Botón 3: LUGARES
                        else if (i == 3) pantallaActual = MENU_TIENDA;      // Botón 4: TIENDA
                        else if (i == 4) pantallaActual = MENU_ACCESORIOS;  // Botón 5: ACCESORIOS
                        else if (i == 5) pantallaActual = MENU_CONFIG;      // Botón 6: CONFIGURACION
                    }
                }
            }
            else if (pantallaActual == MENU_JUEGO) {
                DrawText("JUEGO", 20, 20, 30, DARKGREEN);
                DrawText("Presiona Y para jugar", 20, 60, 20, PURPLE);
                DrawText("Score/100 = Monedas", 20, 80, 20, PURPLE);
                DrawText("Presiona BACKSPACE para volver", 20, 100, 20, GRAY);
                if (IsKeyPressed(KEY_Y)) {
                    juegoRitmico(Pocket); // Llama a la función del juego
                }
                if (IsKeyPressed(KEY_BACKSPACE)) pantallaActual = MENU_PRINCIPAL;
            }
            else if (pantallaActual == MENU_CONFIG) {
                DrawText("CONFIGURACION", 20, 20, 30, DARKGRAY);
                DrawText("Presiona BACKSPACE para volver", 20, 60, 20, GRAY);

                // Volumen
                static float volumen = 0.5f;
                DrawText(TextFormat("Volumen: %.0f%%", volumen * 100), 40, 110, 20, BLACK);
                DrawRectangle(40, 140, 120, 10, GRAY);
                DrawRectangle(40, 140, (int)(volumen * 120), 10, DARKGRAY);
                if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){40, 140, 120, 10}) && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                    volumen = (GetMousePosition().x - 40) / 120.0f;
                    if (volumen < 0) volumen = 0;
                    if (volumen > 1) volumen = 1;
                    SetMasterVolume(volumen);
                }

                // Cambiar nombre
                DrawText("Nombre:", 40, 170, 20, BLACK);
                DrawRectangle(120, 170, 150, 30, WHITE);
                DrawText(Pocket->nombre, 125, 175, 20, DARKGRAY);
                int key = GetCharPressed();
                while (key > 0) {
                    int len = strlen(Pocket->nombre);
                    if (key >= 32 && key <= 125 && len < 31) {
                        Pocket->nombre[len] = (char)key;
                        Pocket->nombre[len + 1] = '\0';
                    }
                    key = GetCharPressed();
                }
                if (IsKeyPressed(KEY_BACKSPACE)) {
                    int len = strlen(Pocket->nombre);
                    if (len > 0) Pocket->nombre[len - 1] = '\0';
                }
                // Aquí deberías guardar el nombre en la estructura de la mascota si la tienes

                // Botón Reiniciar
                DrawRectangle(40, 220, 120, 40, ORANGE);
                DrawText("Reiniciar", 60, 230, 20, WHITE);
                if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){40, 220, 120, 40}) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    reiniciar(&Pocket); // Reinicia la mascota
                    Pocket->escenarioActual = array_get(vectorEscenarios, 0); // Reinicia el escenario
                    pantallaActual = MENU_PRINCIPAL;
                    // Y reiniciar otras variables del juego aquí
                }

                // Botón Salir
                DrawRectangle(180, 220, 120, 40, RED);
                DrawText("Salir", 220, 230, 20, WHITE);
                if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){180, 220, 120, 40}) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    break;
                }
                if (IsKeyPressed(KEY_BACKSPACE)) pantallaActual = MENU_PRINCIPAL;
            }
            else if (pantallaActual == MENU_ACCESORIOS) {
                DrawText("ACCESORIOS", 20, 20, 30, DARKPURPLE);
                cambiarAspectoMascota(Pocket); // cambiar el aspecto de la mascota
                DrawText("Presiona BACKSPACE para volver", 20, 60, 20, GRAY);
                if (IsKeyPressed(KEY_BACKSPACE)) pantallaActual = MENU_PRINCIPAL;
            }
            else if (pantallaActual == MENU_TIENDA) {
                DrawText("TIENDA", 20, 20, 30, ORANGE);
                DrawText("Presiona Y para comprar", 20, 60, 20, PURPLE);
                mostrarTienda(Pocket, array_get(vectorEscenarios, 0)); 
                DrawText("Presiona BACKSPACE para volver", 20, 60, 20, GRAY);
                if (IsKeyPressed(KEY_BACKSPACE)) pantallaActual = MENU_PRINCIPAL;
            }
            else if (pantallaActual == MENU_COMIDA) {
                DrawText("COMIDA", 20, 20, 30, DARKBROWN);
                alimentarMascota(Pocket); 
                DrawText("Presiona BACKSPACE para volver", 20, 60, 20, GRAY);
                if (IsKeyPressed(KEY_BACKSPACE)) pantallaActual = MENU_PRINCIPAL;
            }
            else if (pantallaActual == MENU_LUGARES) {
                DrawText("LUGARES", 20, 20, 30, DARKGRAY);
                cambiarEscenario(Pocket, vectorEscenarios); 
                DrawText("Presiona BACKSPACE para volver", 20, 60, 20, GRAY);
                if (IsKeyPressed(KEY_BACKSPACE)) pantallaActual = MENU_PRINCIPAL;
            }
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadTexture(button);  // Unload button texture
    for (int i = 0; i < NUM_BUTTONS; i++) {
        UnloadTexture(buttonTextures[i]); // Unload each button texture
    }
    UnloadSound(fxButton);  // Unload sound
    UnloadTexture(fondo);
    UnloadTexture(Pocket->aspecto_actual); // Unload pet texture
    free(Pocket->nombre); // Free the pet name string
    UnloadModel(guitarra); // Unload the guitar model
    UnloadModel(fondoModel);
    UnloadModel(skyboxModel);

    CloseAudioDevice();     // Close audio device

    CloseWindow();          // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
