#include "include/ves.hpp"

using namespace VES;

float Context::HeightAtPlanarWorldPos(const glm::vec2& planar_world) {
    const Component::Transform terrain_transform = world.get<Component::Transform>(map->terrain);
    const Component::Terrain terrain = world.get<Component::Terrain>(map->terrain);
    auto translated_x = planar_world.x - terrain_transform.translation.x;
    auto translated_z = planar_world.y - terrain_transform.translation.z;
    uint32_t cell_x = (translated_x / terrain_transform.scale.x) * terrain.heightmap.extent.x;
    uint32_t cell_z = (translated_z / terrain_transform.scale.z) * terrain.heightmap.extent.y;
    uint32_t cell_idx = cell_x + cell_z * terrain.heightmap.extent.x;
    if (cell_idx < terrain.heightmap.extent.x * terrain.heightmap.extent.y) {
        auto height_u8 = terrain.heightmap.data[cell_idx * terrain.heightmap.GetPixelSize()];
        return terrain_transform.translation.y + (height_u8 / 255.0f) * terrain_transform.scale.y;
    }
    return 0.0f;
}

void Context::Dispatch(std::string signal, float delta) {
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
            DrawModel(
                *renderable.model,
                {transform.translation.x,
                    HeightAtPlanarWorldPos(
                        {transform.translation.x,
                            transform.translation.z}),
                    transform.translation.z},
                transform.rotation,
                transform.scale);
        } else {
            DrawModel(
                *renderable.model,
                transform.translation,
                transform.rotation,
                transform.scale);
        }
    });
}

void Context::Update(float delta) {
    UpdateCamera(delta);

    if (!mouse_blocked && GetButtonHeld(MOUSE_BUTTON_LEFT)) {
        world.view<Component::Transform, Component::Blockable, Component::Selectable, Component::LuaBehavior, Component::Name>().each([this, delta](entt::entity entity, Component::Transform& transform, Component::Blockable& blockable, Component::Selectable selectable, Component::LuaBehavior& behavior, Component::Name& name) {
            float height = HeightAtPlanarWorldPos({transform.translation.x, transform.translation.y});
            Bounds real_bounds{
                {transform.translation.x + (blockable.bounds.min.x * transform.scale.x),
                    (height + transform.translation.y) + (blockable.bounds.min.y * transform.scale.y),
                    transform.translation.z + (blockable.bounds.min.z * transform.scale.z)},
                {transform.translation.x + (blockable.bounds.max.x * transform.scale.x),
                    (height + transform.translation.y) + (blockable.bounds.max.y * transform.scale.y),
                    transform.translation.z + (blockable.bounds.max.z * transform.scale.z)}};

            if (CheckMousePick(real_bounds)) {
                if (auto f = behavior.callbacks.find("select"); f != behavior.callbacks.end()) {
                    sol::protected_function_result r = f->second(entity, delta);
                    if (!r.valid()) {
                        sol::error e = r;
                        fmt::print(stderr, "{}", e.what());
                        exit(1);
                    }
                }
                camera.focus = &transform.translation;
                camera.focused_name = &name.name;
            }
        });
    }

    std::fill(map->grid.begin(), map->grid.end(), VES::Cell{});
    world.view<Component::Transform, Component::UnboundedVerticalBlock>().each([this](entt::entity entity, Component::Transform& transform, Component::UnboundedVerticalBlock& block) {
        Bounds bounds = block.bounds;

        for (size_t i = 0; i < static_cast<size_t>((bounds.max.z - bounds.min.z) * transform.scale.z); ++i) {
            for (size_t j = 0; j < static_cast<size_t>((bounds.max.x - bounds.min.x) * transform.scale.x); ++j) {
                size_t index = static_cast<size_t>(transform.translation.x + (bounds.min.x * transform.scale.x) - (map->terrain_transform->translation.x - 1)) + j + (static_cast<size_t>(transform.translation.z + (bounds.min.z * transform.scale.z) - (map->terrain_transform->translation.z - 1)) + i) * map->width;
                if (index < map->width * map->height) {
                    map->grid[index].occupier.emplace(entity);
                }
            }
        }
    });

    Dispatch("update", delta);
}
