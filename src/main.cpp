#include <cstdio>
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

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

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

        world.update(dt);

        BeginDrawing();
        ClearBackground({20, 20, 25, 255});

        world.draw();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}

