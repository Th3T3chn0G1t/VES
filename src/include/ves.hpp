#pragma once

#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>

#include <fmt/format.h>

#include <entt/fwd.hpp>
#include <entt/entt.hpp>

#include <sol/sol.hpp>

#include <yaml-cpp/yaml.h>

#include <glm/glm.hpp>

#include <unordered_map>
#include <filesystem>
#include <algorithm>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <cmath>

namespace VES {
	using Vec2 = Vector2;
	using Vec3 = Vector3;
	using Bounds = BoundingBox;
	using Model = Model;
}

#include "components.hpp"

namespace VES {
    struct Cell {
        std::optional<entt::entity> occupier;
    };

    struct Map {
        Component::Transform* terrain_transform;
        Bounds terrain_bounds;
        entt::entity terrain;
        std::vector<Cell> grid;
        std::size_t width = 0;
        std::size_t height = 0;
    };

    template <typename T>
    struct AssetRegistry {
        std::unordered_map<std::string, T> registry;

        virtual T Load(std::filesystem::path path) = 0;
        T& GetOrLoad(std::filesystem::path path) {
            auto it = registry.find(path.string());
            if (it == registry.end()) {
                return registry[path.string()] = Load(path.string());
            } else {
                return it->second;
            }
        }
    };

    struct ModelRegistry : AssetRegistry<Model> {
        Model Load(std::filesystem::path path) {
            fmt::print("Loading model {}\n", path.string());
            return LoadModel(path.string().c_str());
        }
    };

    struct Camera {
        Vec3 rotation = {0.0f, M_PI_2, 0.0f};
        Vec3 forward = {0.0f, 0.0f, 0.0f};
        Vec3 left = {0.0f, 0.0f, 0.0f};
        Vec3 position = {0.0f, 0.0f, 0.0f};
        float zoom = 40.0f;
        Vec3 target_destination = {0.0f, 0.0f, 0.0f};
        Vec3 position_destination = {0.0f, 0.0f, 0.0f};
        Camera3D camera = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, 45.0f, CAMERA_PERSPECTIVE};

        float move_speed = 20.0f;
        float zoom_speed = 10.0f;
        float turn_speed = 5.0f;
        float min_height = 5.0f;
        Vec3 target_destination_interp_speed = {0.0f, 0.1f, 0.0f};
        Vec3 position_destination_interp_speed = {0.0f, 0.5f, 0.0f};

        Vec2 zoom_limits = {1.0f, 100.0f};

        Vec3* focus = NULL;
        std::string* focused_name = NULL;
    };

    struct Dialog {
        bool shown = false;
        std::string text = "unnamed";
        float current_character = 0;

        static constexpr float speed = 0.5f;
		static Texture2D dialog_texture;
    };

    struct Context {
		struct Config {
			std::filesystem::path datafod = "res";
			Vec2 dim = {640, 480};

			std::filesystem::path initial_map;
		};
		
        std::filesystem::path datafod;
        Vec2 screen_dim;

        Camera camera;
        std::unique_ptr<Map> map;
        entt::registry world;
        std::unordered_map<std::string, entt::entity> scene;

        ModelRegistry model_registry;

        sol::state lua;

		bool mouse_blocked = false;
        Dialog dialog;
       	float dialog_width;
        float dialog_height;
        float dialog_x;
        float dialog_y;
        std::size_t ui_text_scale = 20;

		Context(const Config& config);

		void ShowDialog(const std::string& text);
		std::vector<sol::load_result> LoadMap(const std::filesystem::path& path);
        void Update(float delta);
        void UpdateCamera(float delta);
        void Dispatch(std::string signal, float delta);
		void UpdateUI();
		void DrawUI();
        void DrawWorld();
        float HeightAtPlanarWorldPos(Vec2 planar_world);
        void RegisterLuaNatives();
    };
}
