#pragma once

namespace VES {
    namespace Component {}
    struct TransformComponent {
        Vector3 translation = {0.0f, 0.0f, 0.0f};
        Vector3 rotation = {0.0f, 0.0f, 0.0f};
        Vector3 scale = {1.0f, 1.0f, 1.0f};
    };

    struct RenderableComponent {
        Model* model;
        Color tint = WHITE;
    };

    struct SurfaceObjectComponent {};

    struct TerrainComponent {
        Image heightmap;
    };

    struct BehaviorComponent {
        std::function<Vector2(Context&, entt::entity)> planar_movement;
    };

    struct Selectable {};
}
