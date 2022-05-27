#include "include/components.hpp"
#include "include/ves.hpp"

#include <iostream>

// Note to self: do not mix anything raylib with winapi (-> no global asio headers either :(.)

void UpdateWorld(VESContext& ctx, float delta) {
}

void DrawWorld(VESContext& ctx, float delta) {
    ctx.world.view<TransformComponent, BoxComponent>().each([](TransformComponent transform, BoxComponent box) {
        DrawCube(transform.translation, transform.scale.x, transform.scale.y, transform.scale.z, box.color);
    });

    ctx.world.view<TransformComponent, RenderableComponent>().each([](TransformComponent transform, RenderableComponent renderable) {
        DrawModelEx(*renderable.model, transform.translation, Vector3{1.0f, 0.0f, 0.0f}, transform.rotation.x, transform.scale, renderable.tint);
    });
}

int main(void) {
    VESContext ctx{};

    InitWindow(ctx.screen_dim.x, ctx.screen_dim.y, "VES");
    SetTargetFPS(60);
    SetCameraMode(ctx.camera, CAMERA_CUSTOM);

    TransformComponent terrain_transform = {{-50.0f, 0.0f, -50.0f}, {0.0f, 0.0f, 0.0f}, {100.0f, 20.0f, 100.0f}};
    Image terrain_image = LoadImage("res/texture/heightmap.png");
    Texture2D terrain_texture = LoadTextureFromImage(terrain_image);

    Mesh terrain_mesh = GenMeshHeightmap(terrain_image, Vector3{1.0f, 1.0f, 1.0f});
    Model terrain_model = LoadModelFromMesh(terrain_mesh);

    terrain_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = terrain_texture;
    
    entt::entity terrain = ctx.world.create();
    ctx.world.emplace<TransformComponent>(terrain, terrain_transform);
    ctx.world.emplace<RenderableComponent>(terrain, &terrain_model);

    Model teapot = LoadModel("res/model/teapot.obj");

    entt::entity a = ctx.world.create();
    ctx.world.emplace<TransformComponent>(a, Vector3{10.0f, 0.0f, 0.0f}, Vector3{0.0f, 0.0f, 0.0f}, Vector3{2.0f, 2.0f, 2.0f});
    ctx.world.emplace<RenderableComponent>(a, &teapot, RED);
    entt::entity b = ctx.world.create();
    ctx.world.emplace<TransformComponent>(b, Vector3{-10.0f, 0.0f, 0.0f}, Vector3{0.0f, 0.0f, 0.0f}, Vector3{2.0f, 2.0f, 2.0f});
    ctx.world.emplace<RenderableComponent>(b, &teapot, GREEN);
    entt::entity c = ctx.world.create();
    ctx.world.emplace<TransformComponent>(c, Vector3{0.0f, 0.0f, 10.0f}, Vector3{0.0f, 0.0f, 0.0f}, Vector3{2.0f, 2.0f, 2.0f});
    ctx.world.emplace<RenderableComponent>(c, &teapot, BLUE);
    entt::entity d = ctx.world.create();
    ctx.world.emplace<TransformComponent>(d, Vector3{0.0f, 0.0f, -10.0f}, Vector3{0.0f, 0.0f, 0.0f}, Vector3{2.0f, 2.0f, 2.0f});
    ctx.world.emplace<RenderableComponent>(d, &teapot, YELLOW);

    while (!WindowShouldClose()) {
        float delta = GetFrameTime();
        
        uint32_t current_cell = static_cast<uint32_t>(((ctx.camera.target.x  - terrain_transform.translation.x)/ terrain_transform.scale.x) * terrain_image.width) + terrain_image.width * static_cast<uint32_t>(((ctx.camera.target.z - terrain_transform.translation.z) / terrain_transform.scale.z) * terrain_image.height);
        if (current_cell < terrain_image.width * terrain_image.height) {
            ctx.cam_target_destination.y = terrain_transform.translation.y + (static_cast<uint8_t*>(terrain_image.data)[current_cell * GetPixelDataSize(1, 1, terrain_image.format)] / 255.0f) * terrain_transform.scale.y;
        }

        UpdateCamera(ctx, delta);
        UpdateWorld(ctx, delta);
        BeginDrawing();
        {
            ClearBackground(BLACK);
            BeginMode3D(ctx.camera);
            {
                DrawWorld(ctx, delta);
                DrawCube(Vector3{ctx.camera.target.x, ctx.cam_target_destination.y, ctx.camera.target.z}, 1.0f, 1.0f, 1.0f, RED);
            }
            EndMode3D();
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}