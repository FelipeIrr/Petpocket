#include "juego.h"
#include "botones.h"
#include "raylib.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 650
#define NOTE_SIZE 30
#define HIT_ZONE_Y (SCREEN_HEIGHT - 300)
#define HIT_THRESHOLD 0.15f
#define NOTE_SPEED 300.0f
#define CIRCLE_RADIUS 180.0f

typedef enum { NOTE_NORMAL, NOTE_HOLD } NoteType;

typedef struct {
    float time;         // segundo del beat
    float duration;     // duración para notas extendidas (0 para normales)
    bool hit;
    bool holding;
    NoteType type;
} Beat;

void juegoRitmico(Mascota* mascota) {
    Texture2D metro = LoadTexture("resources/metro.png");
    Texture2D hitmark = LoadTexture("resources/hitmark.png");

    Music music = LoadMusicStream("resources/Nothing.wav");
    if (!IsMusicValid(music)) {
        DrawText("No se encontro el archivo de audio.", 100, 100, 30, RED);
        while (!WindowShouldClose()) { BeginDrawing(); ClearBackground(BLACK); EndDrawing(); }
        CloseWindow();
    }

    float bpm = 188.0f;
    float songLength = GetMusicTimeLength(music);
    int beatCount = (int)(songLength * bpm / 60.0f);
    if (beatCount > 512) beatCount = 1512;
    Beat beatMap[1512];

    // Generar beatmap dinámico con pequeñas variaciones y notas extendidas
    srand((unsigned int)time(NULL));
    for (int i = 0; i < beatCount; i++) {
        float baseTime = (60.0f / bpm) * i;
        float offset = ((rand() % 100) - 50) / 1000.0f; // Variación de hasta ±0.05s
        beatMap[i].time = baseTime + offset;
        beatMap[i].hit = false;
        beatMap[i].holding = false;
        // 1 de cada 8 notas será extendida
        if (i > 0 && rand() % 8 == 0) {
            beatMap[i].type = NOTE_HOLD;
            beatMap[i].duration = 0.7f + (rand() % 30) / 100.0f; // 0.7s a 1.0s
        } else {
            beatMap[i].type = NOTE_NORMAL;
            beatMap[i].duration = 0.0f;
        }
    }

    int score = 0, combo = 0;
    char feedbackText[32] = "";
    int feedbackTimer = 0;
    float globalAngle = 0.0f, squareAngle = 0.0f;
    int barEffectTimer = 0;
    bool salir = false;

    // --- Cuenta regresiva inicial ---
    for (int countdown = 3; countdown >= 0; countdown--) {
        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexturePro(metro, (Rectangle){0,0,metro.width,metro.height}, (Rectangle){0,0,SCREEN_WIDTH,SCREEN_HEIGHT}, (Vector2){0,0}, 0.0f, (Color){255,255,255,120});
        if (countdown > 0) {
            char num[8]; sprintf(num, "%d", countdown);
            int ancho = MeasureText(num, 120);
            DrawText(num, SCREEN_WIDTH/2 - ancho/2, SCREEN_HEIGHT/2 - 60, 120, YELLOW);
        } else {
            int ancho = MeasureText("¡YA!", 100);
            DrawText("¡YA!", SCREEN_WIDTH/2 - ancho/2, SCREEN_HEIGHT/2 - 60, 100, GREEN);
        }
        EndDrawing();
        double start = GetTime();
        while (GetTime() - start < 1.0 && !WindowShouldClose()) {}
    }

    PlayMusicStream(music);

    while (!WindowShouldClose() && !salir) {
        UpdateMusicStream(music);
        float t = GetMusicTimePlayed(music);

        if (IsKeyPressed(KEY_BACKSPACE)) {
            salir = true;
            mascota->monedas += score/1;
            continue;
        }

        globalAngle += 0.01f;
        if (globalAngle > 2*PI) globalAngle -= 2*PI;
        squareAngle += 2.0f;
        if (squareAngle > 360.0f) squareAngle -= 360.0f;

        BeginDrawing();
        ClearBackground(BLACK);

        DrawTexturePro(metro, (Rectangle){0,0,metro.width,metro.height}, (Rectangle){0,0,SCREEN_WIDTH,SCREEN_HEIGHT}, (Vector2){0,0}, 0.0f, (Color){255,255,255,120});

        float cx = SCREEN_WIDTH / 2.0f;
        float cy = SCREEN_HEIGHT / 2.0f;
        float hitAngle = globalAngle;
        float hitX = cx + cosf(hitAngle) * CIRCLE_RADIUS;
        float hitY = cy + sinf(hitAngle) * CIRCLE_RADIUS;

        float baseSize = NOTE_SIZE * 2;
        float squareSize = baseSize + (barEffectTimer > 0 ? NOTE_SIZE * 1.5f : 0);
        Color squareColor = (barEffectTimer > 0) ? GREEN : YELLOW;
        if (barEffectTimer > 0) barEffectTimer--;

        DrawRectanglePro((Rectangle){ hitX, hitY, squareSize, squareSize }, (Vector2){ squareSize/2, squareSize/2 }, squareAngle, squareColor);
        DrawCircle((int)hitX, (int)hitY, NOTE_SIZE, GRAY);
        DrawText("PRESIONA SPACE", SCREEN_WIDTH/2 - 80, 40, 20, RAYWHITE);
        DrawText(TextFormat("Combo: %d", combo), SCREEN_WIDTH/2.2, SCREEN_HEIGHT/2.5, 20, RAYWHITE);
        DrawText("PRESIONA BACKSPACE PARA SALIR", SCREEN_WIDTH/4 - 150, 600, 20, RAYWHITE);

        // Dibujar y chequear notas
        bool noteInRange = false;
        // Lleva registro del último tiempo de nota para evitar solapamientos
        float lastNoteTime = -100.0f;

        for (int i = 0; i < beatCount; i++) {
            if (beatMap[i].hit) continue;

            // Evita solapamiento: si la nota está demasiado cerca de la anterior, sáltala
            if (beatMap[i].time - lastNoteTime < (60.0f / bpm) * 0.5f) continue;
            lastNoteTime = beatMap[i].time;

            float dt = t - beatMap[i].time;
            float noteAngle = globalAngle - dt * 1.7f;
            float noteX = cx + cosf(noteAngle) * CIRCLE_RADIUS;
            float noteY = cy + sinf(noteAngle) * CIRCLE_RADIUS;

            if (dt < -2.0f || dt > 2.0f) continue;

            // Dibuja la textura de la nota
            Color noteColor = (beatMap[i].type == NOTE_HOLD) ? BLUE : WHITE;
            DrawTexturePro(
            hitmark,
            (Rectangle){0, 0, hitmark.width, hitmark.height},
            (Rectangle){
                noteX - NOTE_SIZE,
                noteY - NOTE_SIZE,
                NOTE_SIZE * 2,
                NOTE_SIZE * 2
            },
            (Vector2){0, 0},
            0.0f,
            noteColor
            );

            float diff = fabsf(noteAngle - hitAngle);
            if (diff > PI) diff = 2*PI - diff;

            if (diff < HIT_THRESHOLD) noteInRange = true;

            // --- Notas normales ---
            if (beatMap[i].type == NOTE_NORMAL) {
            if (IsKeyPressed(KEY_SPACE)) {
                if (diff < HIT_THRESHOLD) {
                beatMap[i].hit = true;
                score += (diff < 0.05f ? 300 : 100);
                strcpy(feedbackText, diff < 0.05f ? "EXCELENTE!" : "Bien!");
                combo++;
                feedbackTimer = 30;
                barEffectTimer = 10;
                }
            }
            // No quitar combo si la nota aparece y no se presiona
            if (dt > 0.2f && !beatMap[i].hit) {
                beatMap[i].hit = true;
                strcpy(feedbackText, "Miss :(");
                // combo = 0; // <--- Ya no se reinicia el combo aquí
                feedbackTimer = 30;
                score -= 50;
                if (score < 0) score = 0;
            }
            }
            // --- Notas extendidas ---
            else if (beatMap[i].type == NOTE_HOLD) {
            // Si el jugador presiona y mantiene SPACE dentro del rango
            if (!beatMap[i].holding && diff < HIT_THRESHOLD && IsKeyPressed(KEY_SPACE)) {
                beatMap[i].holding = true;
                beatMap[i].hit = false;
                beatMap[i].time = t; // Marca el inicio del hold
                strcpy(feedbackText, "¡Mantén SPACE!");
                feedbackTimer = 30;
            }
            // Mientras mantiene SPACE, suma puntos
            if (beatMap[i].holding && IsKeyDown(KEY_SPACE)) {
                float holdTime = t - beatMap[i].time;
                score += 2; // Suma puntos por cada frame
                if (holdTime >= beatMap[i].duration) {
                beatMap[i].hit = true;
                beatMap[i].holding = false;
                strcpy(feedbackText, "¡Hold perfecto!");
                combo++;
                feedbackTimer = 30;
                barEffectTimer = 10;
                }
            }
            // Si suelta antes de tiempo, penaliza
            if (beatMap[i].holding && !IsKeyDown(KEY_SPACE)) {
                float holdTime = t - beatMap[i].time;
                if (holdTime < beatMap[i].duration) {
                beatMap[i].hit = true;
                beatMap[i].holding = false;
                strcpy(feedbackText, "¡Hold incompleto!");
                // combo = 0; // <--- Ya no se reinicia el combo aquí
                feedbackTimer = 30;
                score -= 50;
                if (score < 0) score = 0;
                }
            }
            // Si pasa el tiempo y no se hizo hold
            if (!beatMap[i].holding && dt > beatMap[i].duration + 0.2f) {
                beatMap[i].hit = true;
                strcpy(feedbackText, "Miss hold :(");
                // combo = 0; // <--- Ya no se reinicia el combo aquí
                feedbackTimer = 30;
                score -= 50;
                if (score < 0) score = 0;
            }
            }
        }

        if (IsKeyPressed(KEY_SPACE) && !noteInRange) {
            strcpy(feedbackText, "Muy Pronto!");
            combo = 0;
            feedbackTimer = 30;
            score -= 30;
            if (score < 0) score = 0;
        }

        DrawText(TextFormat("Puntuacion: %04i", score), SCREEN_WIDTH/2.5, SCREEN_HEIGHT/2, 24, GREEN);
        if (feedbackTimer > 0) {
            DrawText(feedbackText, SCREEN_WIDTH/2 - MeasureText(feedbackText, 24)/2, 80, 24, YELLOW);
            feedbackTimer--;
        }

        EndDrawing();
    }

    UnloadTexture(metro);
    UnloadTexture(hitmark);
    StopMusicStream(music);
    UnloadMusicStream(music);
}
