#pragma once

namespace VES {
    namespace Component {
        struct Transform {
            Vector3 translation = {0.0f, 0.0f, 0.0f};
            Vector3 rotation = {0.0f, 0.0f, 0.0f};
            Vector3 scale = {1.0f, 1.0f, 1.0f};
        };

        struct Renderable {
            Model* model;
            Color tint = WHITE;
        };

        struct SurfaceObject {
            char pad;
        };

        struct Terrain {
            Image heightmap;
        };

        struct Behavior {
            using Functor = std::function<void(Context&, entt::entity, float)>;
            using CallbackMap = std::unordered_map<std::string, Functor>;

            CallbackMap callbacks;
        };

        struct LuaBehavior {
            using Functor = sol::protected_function;
            using CallbackMap = std::unordered_map<std::string, Functor>;

            CallbackMap callbacks;
        };

        struct Name {
            std::string name;
        };

        struct UnboundedVerticalBlock {
            char pad;
        };
    }
}
