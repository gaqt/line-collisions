#include <raylib.h>
#define RAYGUI_IMPLEMENTATION
#include "../lib/raygui/raygui.h"

#define WIDTH 700.0f
#define HEIGHT 700.0f
#define PADDING 100.0f
#define LINE_LEN (2.0f * fmaxf(WIDTH, HEIGHT))
#define MIN_LINES 4
#define MAX_LINES 500

typedef struct {
    Vector2 startPos;
    Vector2 endPos;
    float theta;
    int hitIdx;
} Line;

float DistV2(Vector2 a, Vector2 b) {
    return sqrtf(powf(a.x - b.x, 2) + powf(a.y - b.y, 2));
}

void GenerateLines(Line lines[MAX_LINES + 1], float newTheta, int lineCnt) {
    if (lines[3].theta == newTheta)
        return;

    lines[3].theta = newTheta;
    lines[3].hitIdx = -1;

    for (int i = 3; i < lineCnt; i++) {
        float theta = lines[i].theta;
        float dx = LINE_LEN * cosf(theta);
        float dy = LINE_LEN * sinf(theta);

        Vector2 startPos = lines[i - 1].endPos;
        Vector2 overflowPos = {startPos.x + dx, startPos.y - dy};
        Vector2 endPos = overflowPos;

        float nextTheta = -1;
        int nextHitIdx = -1;

        for (int j = 0; j < i - 1; j++) {
            if (j == lines[i].hitIdx)
                continue;

            Vector2 collisionPoint;
            if (!CheckCollisionLines(startPos, overflowPos, lines[j].startPos,
                                     lines[j].endPos, &collisionPoint))
                continue;

            if (DistV2(startPos, collisionPoint) > DistV2(startPos, endPos))
                continue;

            endPos = collisionPoint;
            float theta1 = lines[j].theta;
            float theta2 = theta;
            float theta3;
            theta2 -= theta1;
            if (theta2 < 0.0f)
                theta2 += 2.0f * PI;

            theta3 = 2.0f * PI - theta2;

            theta3 += theta1;
            if (theta3 > 2.0f * PI)
                theta3 -= 2.0f * PI;

            nextTheta = theta3;
            nextHitIdx = j;
        }

        lines[i].startPos = startPos;
        lines[i].endPos = endPos;
        lines[i + 1].hitIdx = nextHitIdx;
        lines[i + 1].theta = nextTheta;
    }
}

int main(void) {

    InitWindow(WIDTH, HEIGHT, "Line Collisions");
    SetTargetFPS(60);
    GuiLoadStyle("assets/style_terminal.rgs");

    Line lines[MAX_LINES + 1];

    lines[0] = (Line){{PADDING, PADDING}, {PADDING, HEIGHT - PADDING}, 1.5f * PI, 0};
    lines[1] = (Line){{PADDING, HEIGHT - PADDING},
                {WIDTH - PADDING, HEIGHT - PADDING},
                0.0f,
                0};
    lines[2] = (Line){{WIDTH - PADDING, HEIGHT - PADDING},
                {WIDTH - PADDING, PADDING},
                PI / 2.0f,
                0};

    int lineCnt = 100;
    float newTheta = PI * 1.2f;

    char minLinesText[10];
    char maxLinesText[10];
    sprintf(minLinesText, "%d", MIN_LINES);
    sprintf(maxLinesText, "%d", MAX_LINES);

    while (!WindowShouldClose()) {

        // ---- Update ----

        GenerateLines(lines, newTheta, lineCnt);

        // ---- Drawing ----

        BeginDrawing();

        ClearBackground(BLACK);
        GuiSlider((Rectangle){10, 10, 200, 50}, "180", "210", &newTheta,
                  PI, 1.25f * PI);
        for (int i = 0; i < lineCnt; i++) {
            DrawLineV(lines[i].startPos, lines[i].endPos, GREEN);
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
