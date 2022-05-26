#include "raylib.h"
#include "raymath.h"
#include <cmath>
#include <fmt/format.h>
#include <entt/entt.hpp>

// Note to self: do not mix anything raylib with winapi (-> no global asio headers either :(.)

struct VESContext {
    Vector2 screen_dim = {640, 480};
    Vector3 cam_rotation = {0.0f, 0.0f, 0.0f};
    Vector3 cam_forward = {0.0f, 0.0f, 0.0f};
    Vector3 cam_left = {0.0f, 0.0f, 0.0f};
    Vector3 cam_position = {0.0f, 10.0f, 0.0f};
    float cam_zoom = 1.0f;
    Camera3D camera = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, 45.0f, CAMERA_PERSPECTIVE};
    float mouse_sense = 5.0f;
    float move_speed = 0.5f;
    float zoom_speed = 0.1f;
};

void UpdateCamera(VESContext& ctx) {
    auto& camera = ctx.camera;

    ctx.cam_forward = Vector3{static_cast<float>(std::sin(ctx.cam_rotation.x)), static_cast<float>(std::sin(ctx.cam_rotation.y)), static_cast<float>(std::sin(M_PI_2 - ctx.cam_rotation.x))};
#ifdef VES_DO_FLYCAM

    if (IsKeyDown(KEY_Q)) {
        ctx.cam_zoom -= ctx.zoom_speed;
    } else if (IsKeyDown(KEY_E)) {
        ctx.cam_zoom += ctx.zoom_speed;
    }

    if (IsKeyDown(KEY_W)) {
        ctx.cam_position = Vector3{ctx.cam_position.x + (ctx.cam_forward.x * ctx.move_speed), ctx.cam_position.y + (ctx.cam_forward.y * ctx.move_speed), ctx.cam_position.z + (ctx.cam_forward.z * ctx.move_speed)};
    } else if (IsKeyDown(KEY_S)) {
        ctx.cam_position = Vector3{ctx.cam_position.x - (ctx.cam_forward.x * ctx.move_speed), ctx.cam_position.y - (ctx.cam_forward.y * ctx.move_speed), ctx.cam_position.z - (ctx.cam_forward.z * ctx.move_speed)};
    }
    if (IsKeyDown(KEY_A)) {
        ctx.cam_position = Vector3{ctx.cam_position.x + (ctx.cam_left.x * ctx.move_speed), ctx.cam_position.y + (ctx.cam_left.y * ctx.move_speed), ctx.cam_position.z + (ctx.cam_left.z * ctx.move_speed)};
    } else if (IsKeyDown(KEY_D)) {
        ctx.cam_position = Vector3{ctx.cam_position.x - (ctx.cam_left.x * ctx.move_speed), ctx.cam_position.y - (ctx.cam_left.y * ctx.move_speed), ctx.cam_position.z - (ctx.cam_left.z * ctx.move_speed)};
    }

    camera.position = Vector3{ctx.cam_position.x * ctx.cam_zoom, ctx.cam_position.y * ctx.cam_zoom, ctx.cam_position.z * ctx.cam_zoom};
    camera.target = Vector3{camera.position.x + ctx.cam_forward.x, camera.position.y + ctx.cam_forward.y, camera.position.z + ctx.cam_forward.z};

    auto mouse_delta = GetMouseDelta();
    ctx.cam_rotation.x += ((-mouse_delta.x / ctx.screen_dim.x) / (2 * M_PI)) * ctx.mouse_sense;
    ctx.cam_rotation.y += ((-mouse_delta.y / ctx.screen_dim.y) / (2 * M_PI)) * ctx.mouse_sense;
    ctx.cam_left = Vector3CrossProduct(camera.up, ctx.cam_forward);
#else
    if (IsKeyDown(KEY_Q)) {
        ctx.cam_zoom -= ctx.zoom_speed;
    } else if (IsKeyDown(KEY_E)) {
        ctx.cam_zoom += ctx.zoom_speed;
    }

    auto mouse_delta = GetMouseDelta();
    ctx.cam_rotation.x += ((-mouse_delta.x / ctx.screen_dim.x) / (2 * M_PI)) * ctx.mouse_sense;
    ctx.cam_rotation.y += ((-mouse_delta.y / ctx.screen_dim.y) / (2 * M_PI)) * ctx.mouse_sense;
    ctx.cam_left = Vector3CrossProduct(camera.up, ctx.cam_forward);

    Vector2 planar_vec_to_cam = Vector2{std::sin(ctx.cam_rotation.x), std::cos(ctx.cam_rotation.x)};
    Vector2 planar_left = Vector2{std::sin(static_cast<float>(ctx.cam_rotation.x + M_PI_2)), std::cos(static_cast<float>(ctx.cam_rotation.x + M_PI_2))};

    if (IsKeyDown(KEY_W)) {
        camera.target.x -= planar_vec_to_cam.x * ctx.move_speed;
        camera.target.z -= planar_vec_to_cam.y * ctx.move_speed;
    } else if (IsKeyDown(KEY_S)) {
        camera.target.x += planar_vec_to_cam.x * ctx.move_speed;
        camera.target.z += planar_vec_to_cam.y * ctx.move_speed;
    }
    if (IsKeyDown(KEY_A)) {
        camera.target.x -= planar_left.x * ctx.move_speed;
        camera.target.z -= planar_left.y * ctx.move_speed;
    } else if (IsKeyDown(KEY_D)) {
        camera.target.x += planar_left.x * ctx.move_speed;
        camera.target.z += planar_left.y * ctx.move_speed;
    }

    camera.position = Vector3{camera.target.x + (planar_vec_to_cam.x * ctx.cam_zoom), ctx.cam_position.y + (std::sin(ctx.cam_rotation.y) * ctx.cam_zoom), camera.target.z + (planar_vec_to_cam.y * ctx.cam_zoom)};
#endif

    UpdateCamera(&camera);
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
                DrawCube(Vector3{0.0f, -1.5f, 0.0f}, 100.0f, 1.0f, 100.0f, GRAY);
                DrawCube(Vector3{10.0f, 0.0f, 0.0f}, 2.0f, 2.0f, 2.0f, RED);
                DrawCube(Vector3{0.0f, 0.0f, -10.0f}, 2.0f, 2.0f, 2.0f, BLUE);
                DrawCube(Vector3{0.0f, 0.0f, 10.0f}, 2.0f, 2.0f, 2.0f, GREEN);
                DrawCube(Vector3{-10.0f, 0.0f, 0.0f}, 2.0f, 2.0f, 2.0f, YELLOW);
            }
            EndMode3D();
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}