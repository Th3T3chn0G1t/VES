#pragma once

#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>

#include <fmt/format.h>

#include <entt/fwd.hpp>
#include <entt/entt.hpp>

#include <sol/sol.hpp>

#include <unordered_map>
#include <filesystem>
#include <algorithm>
#include <functional>
#include <memory>
#include <cmath>

namespace VES {
    struct Map {
        entt::entity terrain;
    };

    struct Camera {
        Vector3 cam_rotation = {0.0f, M_PI_2, 0.0f};
        Vector3 cam_forward = {0.0f, 0.0f, 0.0f};
        Vector3 cam_left = {0.0f, 0.0f, 0.0f};
        Vector3 cam_position = {0.0f, 0.0f, 0.0f};
        float cam_zoom = 40.0f;
        Vector3 cam_target_destination = {0.0f, 0.0f, 0.0f};
        Vector3 cam_position_destination = {0.0f, 0.0f, 0.0f};
        Camera3D camera = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, 45.0f, CAMERA_PERSPECTIVE};

        float move_speed = 20.0f;
        float zoom_speed = 10.0f;
        float turn_speed = 5.0f;
        float cam_min_height = 5.0f;
        Vector3 cam_target_destination_interp_speed = {0.0f, 0.1f, 0.0f};
        Vector3 cam_position_destination_interp_speed = {0.0f, 0.5f, 0.0f};

        Vector2 zoom_limits = {1.0f, 100.0f};
    };

    struct Context {
        std::filesystem::path datafod = "res";
        Vector2 screen_dim = {1280, 720};

        Camera camera;
        Map* map;
        entt::registry world;
        std::unordered_map<std::string, entt::entity> scene;

        sol::state lua;

        void UpdateCamera(float delta);
        void Dispatch(std::string signal, float delta);
        void DrawWorld(float delta);
        float HeightAtPlanarWorldPos(Vector2 planar_world);
        void RegisterLuaNatives();
    };
}

#include "components.hpp"
