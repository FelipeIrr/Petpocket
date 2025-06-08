/*******************************************************************************************
*
*   raylib [textures] example - sprite button
*
*   Example complexity rating: [★★☆☆] 2/4
*
*   Example originally created with raylib 2.5, last time updated with raylib 2.5
*
*   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software
*
*   Copyright (c) 2019-2025 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"
#include <math.h>

#define NUM_FRAMES  3       // Number of frames (rectangles) for the button sprite texture
#define NUM_BUTTONS 6

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
        { 100, 100, (float)button.width, frameHeight }, // Botón 1 en (100, 100)
        { 300, 200, (float)button.width, frameHeight }, // Botón 2 en (300, 200)
        { 500, 300, (float)button.width, frameHeight }, // Botón 3 en (500, 300)
        { 100, 200, (float)button.width, frameHeight }, // Botón 4 en (100, 200)
        { 300, 300, (float)button.width, frameHeight }, // Botón 5 en (300, 300)
        { 500, 100, (float)button.width, frameHeight }  // Botón 6 en (500, 100)
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
        angle += 0.01f;
        camera.position.x = sinf(angle) * 10.0f;
        camera.position.z = cosf(angle) * 10.0f;
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);
            for (int i = 0; i < NUM_BUTTONS; i++) {
                sourceRec.y = btnState[i]*frameHeight;
                DrawTextureRec(button, sourceRec, (Vector2){ btnBounds[i].x, btnBounds[i].y }, WHITE);
            }

            BeginMode3D(camera);
                rlDisableBackfaceCulling(); // Permite ver la textura desde dentro del cubo
                DrawModel(skyboxModel, (Vector3){0,0,0}, 1.0f, WHITE);
                rlEnableBackfaceCulling();  // Vuelve a activar el culling
                DrawBillboard(camera, imagen2D, (Vector3){0,1,0}, 2.0f, WHITE);
            EndMode3D();

            DrawText("Camara girando alrededor de la imagen 2D", 10, 10, 20, DARKGRAY);
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
    // Al final, descarga el modelo del skybox
    UnloadModel(skyboxModel);

    CloseAudioDevice();     // Close audio device

    CloseWindow();          // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}