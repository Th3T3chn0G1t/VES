#pragma once

#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>

#include <fmt/format.h>

#include <entt/fwd.hpp>
#include <entt/entt.hpp>

#include <unordered_map>
#include <filesystem>
#include <cmath>

struct VESContext {
    Vector2 screen_dim = {1280, 720};

    Vector3 cam_rotation = {0.0f, 0.0f, 0.0f};
    Vector3 cam_forward = {0.0f, 0.0f, 0.0f};
    Vector3 cam_left = {0.0f, 0.0f, 0.0f};
    Vector3 cam_position = {0.0f, 10.0f, 0.0f};
    float cam_zoom = 10.0f;
    Camera3D camera = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, 45.0f, CAMERA_PERSPECTIVE};

    float move_speed = 20.0f;
    float zoom_speed = 10.0f;
    float turn_speed = 5.0f;

    Vector2 zoom_limits = {1.0f, 100.0f};

    entt::registry world;
};

void UpdateCamera(VESContext& ctx, float delta);
