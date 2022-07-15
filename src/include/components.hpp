#pragma once

namespace VES {
    namespace Component {
        struct Transform {
            glm::vec3 translation = {0.0f, 0.0f, 0.0f};
            glm::vec3 rotation = {0.0f, 0.0f, 0.0f};
            glm::vec3 scale = {1.0f, 1.0f, 1.0f};

            operator glm::mat4();
        };

        struct Renderable {
            Model* model;
        };

        struct SurfaceObject {
            char pad;
        };

        struct Terrain {
        	Texture heightmap;
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
            Bounds bounds;

            UnboundedVerticalBlock(Model& model) : bounds(model.GetBounds()) {};
        };

        struct Blockable {
            Bounds bounds;

            Blockable(Model& model) : bounds(model.GetBounds()) {};
        };

        struct Selectable {
            char pad;
        };
    }
}
