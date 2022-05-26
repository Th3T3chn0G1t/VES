#include "raylib.h"
#include <cmath>

int main(void) {
    float mouse_sense = 5.0f;

    Vector2 screen_dim = {640, 480};
    InitWindow(screen_dim.x, screen_dim.y, "VES");
    SetTargetFPS(60);

    Vector3 camera_rotation = {0.0f, 0.0f, 0.0f};
    Camera3D camera = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, 45.0f, CAMERA_PERSPECTIVE};

    SetCameraMode(camera, CAMERA_CUSTOM);

    DisableCursor();

    while(!WindowShouldClose()) {
        camera.target = (Vector3) {camera.position.x + static_cast<float>(std::sin(camera_rotation.x)), camera.position.y + static_cast<float>(std::tan(camera_rotation.y)), camera.position.z + static_cast<float>(std::sin(M_PI_2 - camera_rotation.x))};

        Vector2 mouse_delta = GetMouseDelta();
        camera_rotation.x += ((-mouse_delta.x / screen_dim.x) / (2 * M_PI)) * mouse_sense;
        camera_rotation.y += ((-mouse_delta.y / screen_dim.y) / (2 * M_PI)) * mouse_sense;

        UpdateCamera(&camera);

        BeginDrawing();
            ClearBackground(RAYWHITE);

            BeginMode3D(camera);
                DrawCube((Vector3) {10.0f, 0.0f, 0.0f}, 2.0f, 2.0f, 2.0f, RED);
                DrawCube((Vector3) {0.0f, 0.0f, -10.0f}, 2.0f, 2.0f, 2.0f, BLUE);
                DrawCube((Vector3) {0.0f, 0.0f, 10.0f}, 2.0f, 2.0f, 2.0f, GREEN);
                DrawCube((Vector3) {-10.0f, 0.0f, 0.0f}, 2.0f, 2.0f, 2.0f, YELLOW);
            EndMode3D();
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
