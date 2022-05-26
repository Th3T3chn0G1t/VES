#include "raylib.h"
#include "raymath.h"
#include <cmath>
#include <fmt/format.h>

// Note to self: do not mix anything raylib with winapi (-> no global asio headers either :(.)

int main(void) {
    float mouse_sense = 5.0f;
    float move_speed = 0.5f;

    Vector2 screen_dim = {640, 480};
    InitWindow(screen_dim.x, screen_dim.y, "VES");
    SetTargetFPS(60);

    Vector3 camera_rotation = {0.0f, 0.0f, 0.0f};
    Vector3 camera_forward = {0.0f, 0.0f, 0.0f};
    Vector3 camera_left = {0.0f, 0.0f, 0.0f};
    Camera3D camera = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, 45.0f, CAMERA_PERSPECTIVE};

    SetCameraMode(camera, CAMERA_CUSTOM);
    DisableCursor();

    while (!WindowShouldClose()) {
        // * Camera
        {
            camera_forward = (Vector3){static_cast<float>(std::sin(camera_rotation.x)), static_cast<float>(std::tan(camera_rotation.y)), static_cast<float>(std::sin(M_PI_2 - camera_rotation.x))};

            if (IsKeyDown(KEY_W)) {
                camera.position = (Vector3){camera.position.x + (camera_forward.x * move_speed), camera.position.y + (camera_forward.y * move_speed), camera.position.z + (camera_forward.z * move_speed)};
            } else if (IsKeyDown(KEY_S)) {
                camera.position = (Vector3){camera.position.x - (camera_forward.x * move_speed), camera.position.y - (camera_forward.y * move_speed), camera.position.z - (camera_forward.z * move_speed)};
            }
            if (IsKeyDown(KEY_A)) {
                camera.position = (Vector3){camera.position.x + (camera_left.x * move_speed), camera.position.y + (camera_left.y * move_speed), camera.position.z + (camera_left.z * move_speed)};
            } else if (IsKeyDown(KEY_D)) {
                camera.position = (Vector3){camera.position.x - (camera_left.x * move_speed), camera.position.y - (camera_left.y * move_speed), camera.position.z - (camera_left.z * move_speed)};
            }

            camera.target = (Vector3){camera.position.x + camera_forward.x, camera.position.y + camera_forward.y, camera.position.z + camera_forward.z};

            Vector2 mouse_delta = GetMouseDelta();
            camera_rotation.x += ((-mouse_delta.x / screen_dim.x) / (2 * M_PI)) * mouse_sense;
            camera_rotation.y += ((-mouse_delta.y / screen_dim.y) / (2 * M_PI)) * mouse_sense;

            camera_left = Vector3CrossProduct(camera.up, camera_forward);

            UpdateCamera(&camera);
        }

        // * Draw
        BeginDrawing();
        {
            ClearBackground(BLACK);
            BeginMode3D(camera);
            {
                DrawCube((Vector3){10.0f, 0.0f, 0.0f}, 2.0f, 2.0f, 2.0f, RED);
                DrawCube((Vector3){0.0f, 0.0f, -10.0f}, 2.0f, 2.0f, 2.0f, BLUE);
                DrawCube((Vector3){0.0f, 0.0f, 10.0f}, 2.0f, 2.0f, 2.0f, GREEN);
                DrawCube((Vector3){-10.0f, 0.0f, 0.0f}, 2.0f, 2.0f, 2.0f, YELLOW);
            }
            EndMode3D();
        }
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
