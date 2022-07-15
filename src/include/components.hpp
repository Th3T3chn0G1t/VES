#pragma once

namespace VES {
    namespace Component {
        struct Transform {
            Vec3 translation = {0.0f, 0.0f, 0.0f};
            Vec3 rotation = {0.0f, 0.0f, 0.0f};
            Vec3 scale = {1.0f, 1.0f, 1.0f};

            operator Matrix() {
                Matrix ret = MatrixIdentity();
                ret = MatrixMultiply(ret, MatrixTranslate(translation.x, translation.y, translation.z));
                ret = MatrixMultiply(ret, MatrixRotate(Vector3{1.0f, 0.0f, 0.0f}, rotation.x));
                ret = MatrixMultiply(ret, MatrixRotate(Vector3{0.0f, 1.0f, 0.0f}, rotation.y));
                ret = MatrixMultiply(ret, MatrixRotate(Vector3{0.0f, 0.0f, 1.0f}, rotation.z));
                ret = MatrixMultiply(ret, MatrixScale(scale.x, scale.y, scale.z));
                return ret;
            }
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

        struct LuaBehavior {
            using Functor = sol::protected_function;
            using CallbackMap = std::unordered_map<std::string, Functor>;

            CallbackMap callbacks;
        };

        struct Name {
            std::string name;
        };

        struct UnboundedVerticalBlock {
            BoundingBox bounds;

            UnboundedVerticalBlock(Model& model) : bounds(GetModelBoundingBox(model)) {};
        };

        struct Blockable {
            BoundingBox bounds;

            Blockable(Model& model) : bounds(GetModelBoundingBox(model)) {};
        };

        struct Selectable {
            char pad;
        };
    }
}
