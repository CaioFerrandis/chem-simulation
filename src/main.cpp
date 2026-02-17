#include <cstdio>
#include <string>
#include <vector>

#include "raylib.h"

#define WIDTH 800
#define HEIGHT 600

#include "particle.h"
#include "world.h"

int main() {
    SetTraceLogLevel(LOG_WARNING);
    InitWindow(WIDTH, HEIGHT, "Particles");
    SetTargetFPS(60);

    World world;

    float dt_mult = 1.0f;
    while (!WindowShouldClose()) {
        float dt = 0.0f; //GetFrameTime()*dt_mult;

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mouse = GetMousePosition();
            world.electrons.push_back(create_electron(mouse));
        }

        if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
            Vector2 mouse = GetMousePosition();
            world.nucleus.push_back(create_nucleus(mouse, 6));
        }

        if (IsKeyPressed(KEY_H)){
            Vector2 mouse = GetMousePosition();
            world.nucleus.push_back(create_nucleus(mouse, 1));
        }

        dt_mult += GetMouseWheelMove()/100.0;

        world.update(dt);

        BeginDrawing();
        ClearBackground({20, 20, 25, 255});

        world.draw();

        std::string text = std::to_string(dt_mult);
        DrawText(text.c_str(), 10.0, 10.0, 15, RED);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
