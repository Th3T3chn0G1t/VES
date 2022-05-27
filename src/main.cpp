#include "include/ves.hpp"

// Note to self: do not mix anything raylib with winapi (-> no global asio headers either :(.)

int main(int argc, const char** argv) {
    VES::Context ctx{};

    ctx.lua.open_libraries(sol::lib::base, sol::lib::package);
    ctx.lua.script("print(\"Hello, world!\")");
 
    if (argc > 1) {
        ctx.datafod = argv[1];
    } else {
        if (!std::filesystem::is_directory(ctx.datafod)) {
            ctx.datafod = "../res";
        }
    }

    if (!std::filesystem::is_directory(ctx.datafod)) {
        fmt::print("Missing resfolder!");
        std::abort();
    }

    InitWindow(ctx.screen_dim.x, ctx.screen_dim.y, "VES");
    SetTargetFPS(60);
    SetCameraMode(ctx.camera.camera, CAMERA_CUSTOM);

    Image terrain_image = LoadImage(fmt::format("{}/texture/heightmap.png", ctx.datafod.string()).c_str());
    Texture2D terrain_texture = LoadTextureFromImage(terrain_image);
    Mesh terrain_mesh = GenMeshHeightmap(terrain_image, Vector3{1.0f, 1.0f, 1.0f});
    Model terrain_model = LoadModelFromMesh(terrain_mesh);
    terrain_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = terrain_texture;

    entt::entity terrain = ctx.world.create();
    ctx.world.emplace<VES::TransformComponent>(terrain, Vector3{-50.0f, 0.0f, -50.0f}, Vector3{0.0f, 0.0f, 0.0f}, Vector3{100.0f, 20.0f, 100.0f});
    ctx.world.emplace<VES::TerrainComponent>(terrain, terrain_image);
    ctx.world.emplace<VES::RenderableComponent>(terrain, &terrain_model);
    VES::Map map{terrain};
    ctx.map = &map;

    Model teapot = LoadModel(fmt::format("{}/model/teapot.obj", ctx.datafod.string()).c_str());

    entt::entity a = ctx.world.create();
    ctx.world.emplace<VES::TransformComponent>(a, Vector3{10.0f, 0.0f, 0.0f}, Vector3{0.0f, 0.0f, 0.0f}, Vector3{0.5f, 0.5f, 0.5f});
    ctx.world.emplace<VES::RenderableComponent>(a, &teapot, RED);
    ctx.world.emplace<VES::SurfaceObjectComponent>(a);
    ctx.world.emplace<VES::BehaviorComponent>(a, [](VES::Context& ctx, entt::entity entity) {
        return Vector2{(GetRandomValue(0, 255) / 255.0f) - 0.5f, (GetRandomValue(0, 255) / 255.0f) - 0.5f};
    });

    entt::entity b = ctx.world.create();
    ctx.world.emplace<VES::TransformComponent>(b, Vector3{-10.0f, 0.0f, 0.0f}, Vector3{0.0f, 0.0f, 0.0f}, Vector3{0.5f, 0.5f, 0.5f});
    ctx.world.emplace<VES::RenderableComponent>(b, &teapot, GREEN);
    ctx.world.emplace<VES::SurfaceObjectComponent>(b);
    ctx.world.emplace<VES::BehaviorComponent>(b, [](VES::Context& ctx, entt::entity entity) {
        VES::TransformComponent transform = ctx.world.get<VES::TransformComponent>(entity);

        return Vector2{(ctx.camera.camera.target.x - transform.translation.x) * 0.1f, (ctx.camera.camera.target.z - transform.translation.z) * 0.1f};
    });

    entt::entity c = ctx.world.create();
    ctx.world.emplace<VES::TransformComponent>(c, Vector3{0.0f, 0.0f, 10.0f}, Vector3{0.0f, 0.0f, 0.0f}, Vector3{0.5f, 0.5f, 0.5f});
    ctx.world.emplace<VES::RenderableComponent>(c, &teapot, BLUE);
    ctx.world.emplace<VES::SurfaceObjectComponent>(c);
    entt::entity d = ctx.world.create();
    ctx.world.emplace<VES::TransformComponent>(d, Vector3{0.0f, 0.0f, -10.0f}, Vector3{0.0f, 0.0f, 0.0f}, Vector3{0.5f, 0.5f, 0.5f});
    ctx.world.emplace<VES::RenderableComponent>(d, &teapot, YELLOW);
    ctx.world.emplace<VES::SurfaceObjectComponent>(d);

    while (!WindowShouldClose()) {
        float delta = GetFrameTime();

        ctx.camera.cam_target_destination.y = ctx.HeightAtPlanarWorldPos(Vector2{ctx.camera.camera.target.x, ctx.camera.camera.target.z});

        ctx.UpdateCamera(delta);
        ctx.UpdateWorld(delta);
        BeginDrawing();
        {
            ClearBackground(BLACK);
            BeginMode3D(ctx.camera.camera);
            {
                ctx.DrawWorld(delta);
                DrawCube(Vector3{ctx.camera.camera.target.x, ctx.camera.cam_target_destination.y, ctx.camera.camera.target.z}, 1.0f, 1.0f, 1.0f, RED);
            }
            EndMode3D();
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}