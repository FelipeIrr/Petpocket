#include "raylib.h"
#include <stdio.h>
#include <math.h>
#include <string.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define NOTE_SIZE 30
#define HIT_ZONE_Y (SCREEN_HEIGHT - 100)
#define HIT_THRESHOLD 0.15f
#define NOTE_SPEED 300.0f

typedef struct {
    float time;     // segundo del beat
    bool hit;
} Beat;

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Rhythm Game");
    InitAudioDevice();
    SetTargetFPS(60);

    Music music = LoadMusicStream("resources/Nothing.wav");
    if (!IsMusicValid(music)) {
        DrawText("No se encontro el archivo de audio.", 100, 100, 30, RED);
        while (!WindowShouldClose()) { BeginDrawing(); ClearBackground(BLACK); EndDrawing(); }
        CloseWindow();
        return 1;
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

    while (!WindowShouldClose()) {
        UpdateMusicStream(music);
        float t = GetMusicTimePlayed(music);

        BeginDrawing();
        ClearBackground(BLACK);

        // Dibujo zona de impacto
        DrawRectangle(0, HIT_ZONE_Y - NOTE_SIZE/2, SCREEN_WIDTH, NOTE_SIZE, GRAY);
        DrawText("PRESS SPACE", SCREEN_WIDTH/2 - 80, HIT_ZONE_Y + 40, 20, RAYWHITE);

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

        EndDrawing();
    }

    UnloadMusicStream(music);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
