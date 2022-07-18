#pragma once

// TODO: Raylib
#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>

#include <fmt/format.h>

#include <entt/fwd.hpp>
#include <entt/entt.hpp>

#include <sol/sol.hpp>

#include <yaml-cpp/yaml.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <stlastar.h>

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
	struct Bounds {
		glm::vec3 min;
		glm::vec3 max;		
	};

	struct Texture {
		// TODO: Raylib
        Image image;
		Texture2D texture;

		glm::ivec2 extent;
		unsigned char* data;

		Texture() = default;
		Texture(const std::filesystem::path& path);

		std::size_t GetPixelSize() const;
	};

	struct Model {
		// TODO: Raylib
		::Model model;

		Model() = default;
		Model(const std::filesystem::path& path);
		Model(const Texture& heightmap);

		Bounds GetBounds() const;
	};

	// TODO: Raylib
	using Key = KeyboardKey;
	using Button = MouseButton;
}

#include "components.hpp"

namespace VES {
    struct Cell {
        std::optional<entt::entity> occupier;
    };

    struct Map {
    	Model terrain_model;
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
                return registry[path.string()] = Load(path);
            } else {
                return it->second;
            }
        }
    };

    struct ModelRegistry : AssetRegistry<Model> {
        Model Load(std::filesystem::path path) {
            fmt::print("Loading model {}\n", path.string());
            return Model(path);
        }
    };

    struct TextureRegistry : AssetRegistry<Texture> {
        Texture Load(std::filesystem::path path) {
            fmt::print("Loading texture {}\n", path.string());
            return Texture(path);
        }
    };

    struct Camera {
        glm::vec3 rotation = {0.0f, M_PI_2, 0.0f};
        glm::vec3 forward = {0.0f, 0.0f, 0.0f};
        glm::vec3 left = {0.0f, 0.0f, 0.0f};
        glm::vec3 position = {0.0f, 0.0f, 0.0f};
        float zoom = 40.0f;
        glm::vec3 target_destination = {0.0f, 0.0f, 0.0f};
        glm::vec3 position_destination = {0.0f, 0.0f, 0.0f};

		// TODO: Raylib
		//       We probably need to redo all of the camera once we are no
		//       Longer using Raylib.
        Camera3D camera = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, 45.0f, CAMERA_PERSPECTIVE};

        float move_speed = 20.0f;
        float zoom_speed = 10.0f;
        float turn_speed = 5.0f;
        float min_height = 5.0f;
        glm::vec3 target_destination_interp_speed = {0.0f, 0.1f, 0.0f};
        glm::vec3 position_destination_interp_speed = {0.0f, 0.5f, 0.0f};

        glm::vec2 zoom_limits = {1.0f, 100.0f};

        glm::vec3* focus = nullptr;
        std::string* focused_name = nullptr;
    };

    struct Dialog {
        bool shown = false;
        std::string text = "unnamed";
        float current_character = 0;
		Texture* dialog_texture = nullptr;

        static constexpr float speed = 0.5f;
    };

    struct Window {
    	glm::ivec2 extent;
    
    	Window(const glm::ivec2& extent, const std::string& name);	
    	~Window();

    	bool GetShouldClose();
    };

    struct Context {
		struct Config {
			std::string name{"VES"};
			std::filesystem::path datafod{"res"};
			glm::ivec2 dim{640, 480};

			std::filesystem::path initial_map;

			Config(const std::filesystem::path& path);
		};
		
        std::filesystem::path datafod;

		Window window;

        Camera camera;
        std::unique_ptr<Map> map;
        entt::registry world;
        std::unordered_map<std::string, entt::entity> scene;

        ModelRegistry model_registry;
        TextureRegistry texture_registry;

        sol::state lua;

		bool mouse_blocked = false;
        Dialog dialog;
       	float dialog_width;
        float dialog_height;
        float dialog_x;
        float dialog_y;
        std::size_t ui_text_scale = 20;

		Context(const Config& config);
		~Context();

		void ShowDialog(const std::string& text);
        float HeightAtPlanarWorldPos(const glm::vec2& planar_world);

		std::vector<sol::load_result> LoadMap(const std::filesystem::path& path);
        void RegisterLuaNatives();

		float GetFrameDelta();

        void Update(float delta);
        void UpdateCamera(float delta);
        void Dispatch(std::string signal, float delta);

		void BeginFrame(const glm::vec4& clear);
		void SubmitGeometry();
		void EndFrame();

		void UpdateUI();
		void DrawUI();
        void DrawWorld();

        void DrawCube(const glm::vec3& pos, const glm::vec3& scale, const glm::vec4& color);
		int GetTextWidth(const std::string& text, const int size);
		void DrawText(const std::string& text, const glm::vec2& pos, const int size, const glm::vec4& color);
		void DrawTexture(Texture& texture, const glm::vec2& pos, const glm::vec2& extent);
		void DrawModel(Model& model, const glm::vec3& translation, const glm::vec3& rotation, const glm::vec3& scale);

		bool GetKeyHeld(const Key key);
		bool GetButtonHeld(const Button button);
		bool GetButtonReleased(const Button button);
		glm::vec2 GetMousePosition();

		bool CheckMousePick(const Bounds& bounds);
    };
}

// 
