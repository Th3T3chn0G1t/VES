#pragma once

#include "raylib.h"
#include <filesystem>

namespace VES {
    struct TransformComponent {
        Vector3 translation = {0.0f, 0.0f, 0.0f};
        Vector3 rotation = {0.0f, 0.0f, 0.0f};
        Vector3 scale = {1.0f, 1.0f, 1.0f};
    };

    struct BoxComponent {
        Color color;
    };

    struct RenderableComponent {
        Model* model;
        Color tint = WHITE;
    };

    struct TerrainComponent {
        Image heightmap;
    };
}
