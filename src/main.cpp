#include "raylib.h"
#include "raymath.h"
#include <cmath>
#include <fmt/format.h>

// Note to self: do not mix anything raylib with winapi (-> no global asio headers either :(.)

struct VESContext {
    Vector2 screen_dim = {640, 480};
    Vector3 cam_rotation = {0.0f, 0.0f, 0.0f};
    Vector3 cam_forward = {0.0f, 0.0f, 0.0f};
    Vector3 cam_left = {0.0f, 0.0f, 0.0f};
    Camera3D camera = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, 45.0f, CAMERA_PERSPECTIVE};
    float mouse_sense = 5.0f;
    float move_speed = 0.5f;
};

void UpdateCamera(VESContext& ctx) {
    ctx.cam_forward = (Vector3){static_cast<float>(std::sin(ctx.cam_rotation.x)), static_cast<float>(std::tan(ctx.cam_rotation.y)), static_cast<float>(std::sin(M_PI_2 - ctx.cam_rotation.x))};

    if (IsKeyDown(KEY_W)) {
        ctx.camera.position = (Vector3){ctx.camera.position.x + (ctx.cam_forward.x * ctx.move_speed), ctx.camera.position.y + (ctx.cam_forward.y * ctx.move_speed), ctx.camera.position.z + (ctx.cam_forward.z * ctx.move_speed)};
    } else if (IsKeyDown(KEY_S)) {
        ctx.camera.position = (Vector3){ctx.camera.position.x - (ctx.cam_forward.x * ctx.move_speed), ctx.camera.position.y - (ctx.cam_forward.y * ctx.move_speed), ctx.camera.position.z - (ctx.cam_forward.z * ctx.move_speed)};
    }
    if (IsKeyDown(KEY_A)) {
        ctx.camera.position = (Vector3){ctx.camera.position.x + (ctx.cam_left.x * ctx.move_speed), ctx.camera.position.y + (ctx.cam_left.y * ctx.move_speed), ctx.camera.position.z + (ctx.cam_left.z * ctx.move_speed)};
    } else if (IsKeyDown(KEY_D)) {
        ctx.camera.position = (Vector3){ctx.camera.position.x - (ctx.cam_left.x * ctx.move_speed), ctx.camera.position.y - (ctx.cam_left.y * ctx.move_speed), ctx.camera.position.z - (ctx.cam_left.z * ctx.move_speed)};
    }

    ctx.camera.target = (Vector3){ctx.camera.position.x + ctx.cam_forward.x, ctx.camera.position.y + ctx.cam_forward.y, ctx.camera.position.z + ctx.cam_forward.z};

    Vector2 mouse_delta = GetMouseDelta();
    ctx.cam_rotation.x += ((-mouse_delta.x / ctx.screen_dim.x) / (2 * M_PI)) * ctx.mouse_sense;
    ctx.cam_rotation.y += ((-mouse_delta.y / ctx.screen_dim.y) / (2 * M_PI)) * ctx.mouse_sense;

    ctx.cam_left = Vector3CrossProduct(ctx.camera.up, ctx.cam_forward);

    UpdateCamera(&ctx.camera);
}

int main(void) {
    VESContext vesCtx{};
    InitWindow(vesCtx.screen_dim.x, vesCtx.screen_dim.y, "VES");
    SetTargetFPS(60);
    SetCameraMode(vesCtx.camera, CAMERA_CUSTOM);
    DisableCursor();

    while (!WindowShouldClose()) {
        UpdateCamera(vesCtx);
        BeginDrawing();
        {
            ClearBackground(BLACK);
            BeginMode3D(vesCtx.camera);
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