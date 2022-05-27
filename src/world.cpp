#include "include/ves.hpp"
#include "include/components.hpp"

using namespace VES;

float Context::HeightAtPlanarWorldPos(Vector2 planar_world) {
    const VES::TransformComponent terrain_transform = this->world.get<VES::TransformComponent>(this->map->terrain);
    const VES::TerrainComponent terrain = this->world.get<VES::TerrainComponent>(this->map->terrain);
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

void Context::UpdateWorld(float delta) {

}

void Context::DrawWorld(float delta) {
    this->world.view<VES::TransformComponent, VES::BoxComponent>().each([](VES::TransformComponent transform, VES::BoxComponent box) {
        DrawCube(transform.translation, transform.scale.x, transform.scale.y, transform.scale.z, box.color);
    });

    this->world.view<VES::TransformComponent, VES::RenderableComponent>().each([](VES::TransformComponent transform, VES::RenderableComponent renderable) {
        DrawModelEx(*renderable.model, transform.translation, Vector3{1.0f, 0.0f, 0.0f}, transform.rotation.x, transform.scale, renderable.tint);
    });
}

