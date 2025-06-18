#include "include/raylib.h"
#include <stdio.h>
#include <math.h>
#include <string.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define NOTE_SIZE 30
#define HIT_ZONE_Y (SCREEN_HEIGHT - 300)
#define HIT_THRESHOLD 0.15f
#define NOTE_SPEED 300.0f
#define CIRCLE_RADIUS 180.0f // Radio del círculo de notas

typedef struct {
    float time;     // segundo del beat
    bool hit;
} Beat;

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Rhythm Game");
    InitAudioDevice();
    SetMasterVolume(0.2f); // Ajusta el volumen general
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
    float bpm = 188.0f; // Cambia esto al BPM de tu canción
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

    float globalAngle = 0.0f; // Ángulo global para la rotación de la hitmark
    float squareAngle = 0.0f; // Ángulo de rotación propio del cuadrado
    int barEffectTimer = 0; // Efecto visual para la barra

    while (!WindowShouldClose()) {
        UpdateMusicStream(music);
        float t = GetMusicTimePlayed(music);

        // Incrementa el ángulo global lentamente (hitmark)
        globalAngle += 0.01f;
        if (globalAngle > 2*PI) globalAngle -= 2*PI;

        // Incrementa el ángulo del cuadrado para que rote en su eje
        squareAngle += 2.0f; // velocidad de giro del cuadrado (grados por frame)
        if (squareAngle > 360.0f) squareAngle -= 360.0f;

        BeginDrawing();
        ClearBackground(BLACK);

        // Centro de la pantalla
        float cx = SCREEN_WIDTH / 2.0f;
        float cy = SCREEN_HEIGHT / 2.0f;

        // Calcula la posición de la zona de impacto (en el círculo)
        float hitAngle = globalAngle;
        float hitX = cx + cosf(hitAngle) * CIRCLE_RADIUS;
        float hitY = cy + sinf(hitAngle) * CIRCLE_RADIUS;

        // Tamaño y color del cuadrado según efecto
        float baseSize = NOTE_SIZE * 2;
        float squareSize = baseSize + (barEffectTimer > 0 ? NOTE_SIZE * 1.5f : 0);
        Color squareColor = (barEffectTimer > 0) ? GREEN : YELLOW;

        if (barEffectTimer > 0) barEffectTimer--;

        // Dibuja el cuadrado centrado y rotado en su propio eje en la zona de impacto
        DrawRectanglePro(
            (Rectangle){ hitX, hitY, squareSize, squareSize },
            (Vector2){ squareSize/2, squareSize/2 },
            squareAngle,
            squareColor
        );

        // Dibuja la zona de impacto (círculo gris)
        DrawCircle((int)hitX, (int)hitY, NOTE_SIZE, GRAY);
        DrawText("PRESIONA SPACE", SCREEN_WIDTH/2 - 80, 40, 20, RAYWHITE);

        // Dibujar y chequear notas
        bool noteInRange = false;
        for (int i = 0; i < beatCount; i++) {
            if (beatMap[i].hit) continue;

            float dt = t - beatMap[i].time;
            float noteAngle = globalAngle - dt * 1.7f;
            float noteX = cx + cosf(noteAngle) * CIRCLE_RADIUS;
            float noteY = cy + sinf(noteAngle) * CIRCLE_RADIUS;

            if (dt < -2.0f || dt > 2.0f) continue;

            DrawCircle((int)noteX, (int)noteY, NOTE_SIZE, BLUE);

            float diff = fabsf(noteAngle - hitAngle);
            if (diff > PI) diff = 2*PI - diff;

            if (diff < HIT_THRESHOLD) noteInRange = true;
            if (IsKeyPressed(KEY_SPACE)) {
                if (diff < HIT_THRESHOLD) {
                    beatMap[i].hit = true;
                    score += (diff < 0.05f ? 300 : 100);
                    strcpy(feedbackText, diff < 0.05f ? "EXCELENTE!" : "Bien!");
                    feedbackTimer = 30;
                    barEffectTimer = 10; // Activa el efecto reactivo (tamaño)
                }
            }

            if (dt > 0.2f && !beatMap[i].hit) {
                beatMap[i].hit = true;
                strcpy(feedbackText, "Miss :(");
                feedbackTimer = 30;
                score -= 50;
                if (score < 0) score = 0;
            }
        }

        if (IsKeyPressed(KEY_SPACE) && !noteInRange) {
            strcpy(feedbackText, "Muy Pronto!");
            feedbackTimer = 30;
            score -= 30;
            if (score < 0) score = 0;
        }

        DrawText(TextFormat("Puntuacion: %04i", score), 10, 10, 24, GREEN);
        if (feedbackTimer > 0) {
            DrawText(feedbackText, SCREEN_WIDTH/2 - MeasureText(feedbackText, 24)/2, 80, 24, YELLOW);
            feedbackTimer--;
        }

        EndDrawing();
    }

    UnloadMusicStream(music);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
