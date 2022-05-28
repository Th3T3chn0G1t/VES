#include "include/ves.hpp"

using namespace VES;

float Context::HeightAtPlanarWorldPos(Vector2 planar_world) {
    const Component::Transform terrain_transform = world.get<Component::Transform>(map->terrain);
    const Component::Terrain terrain = world.get<Component::Terrain>(map->terrain);
    auto translated_x = planar_world.x - terrain_transform.translation.x;
    auto translated_z = planar_world.y - terrain_transform.translation.z;
    uint32_t cell_x = (translated_x / terrain_transform.scale.x) * terrain.heightmap.width;
    uint32_t cell_z = (translated_z / terrain_transform.scale.z) * terrain.heightmap.height;
    uint32_t cell_idx = cell_x + cell_z * terrain.heightmap.width;
    if (cell_idx < terrain.heightmap.width * terrain.heightmap.height) {
        auto height_u8 = static_cast<uint8_t*>(terrain.heightmap.data)[cell_idx * GetPixelDataSize(1, 1, terrain.heightmap.format)];
        return terrain_transform.translation.y + (height_u8 / 255.0f) * terrain_transform.scale.y;
    }
    return 0.0f;
}

void Context::Dispatch(std::string signal, float delta) {
    world.view<Component::Behavior>().each([this, &signal, delta](entt::entity entity, Component::Behavior& behavior) {
        if (auto f = behavior.callbacks.find(signal); f != behavior.callbacks.end()) {
            f->second(*this, entity, delta);
        }
    });

    world.view<Component::LuaBehavior>().each([&signal, delta](entt::entity entity, Component::LuaBehavior& behavior) {
        if (auto f = behavior.callbacks.find(signal); f != behavior.callbacks.end()) {
            sol::protected_function_result r = f->second(entity, delta);
            if (!r.valid()) {
                sol::error e = r;
                fmt::print(stderr, "{}", e.what());
                exit(1);
            }
        }
    });
}

void Context::DrawWorld() {
    world.view<Component::Transform, Component::Renderable>().each([this](entt::entity entity, Component::Transform transform, Component::Renderable renderable) {
        if (world.all_of<Component::SurfaceObject>(entity)) {
            DrawModelEx(*renderable.model, Vector3{transform.translation.x, HeightAtPlanarWorldPos(Vector2{transform.translation.x, transform.translation.z}) + transform.translation.y, transform.translation.z}, Vector3{1.0f, 0.0f, 0.0f}, transform.rotation.x, transform.scale, renderable.tint);
        } else {
            DrawModelEx(*renderable.model, transform.translation, Vector3{1.0f, 0.0f, 0.0f}, transform.rotation.x, transform.scale, renderable.tint);
        }
    });
}

void Context::Update(float delta) {
    UpdateCamera(delta);

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        world.view<Component::Transform, Component::Blockable, Component::Selectable, Component::LuaBehavior>().each([this, delta](entt::entity entity, Component::Transform& transform, Component::Blockable& blockable, Component::Selectable selectable, Component::LuaBehavior& behavior) {
            float height = HeightAtPlanarWorldPos(Vector2{transform.translation.x, transform.translation.y});
            BoundingBox real_bounds = BoundingBox{
                {transform.translation.x + (blockable.bounds.min.x * transform.scale.x),
                    (height + transform.translation.y) + (blockable.bounds.min.y * transform.scale.y),
                    transform.translation.z + (blockable.bounds.min.z * transform.scale.z)},
                {transform.translation.x + (blockable.bounds.max.x * transform.scale.x),
                    (height + transform.translation.y) + (blockable.bounds.max.y * transform.scale.y),
                    transform.translation.z + (blockable.bounds.max.z * transform.scale.z)}};

            if (GetRayCollisionBox(GetMouseRay(GetMousePosition(), camera.camera), real_bounds).hit) {
                behavior.callbacks["select"](entity, delta);
            }
        });
    }

    Dispatch("update", delta);
}
