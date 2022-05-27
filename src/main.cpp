#include "include/ves.hpp"

// Note to self: do not mix anything raylib with winapi (-> no global asio headers either :(.)

static int lua_exception_handler(lua_State* state, sol::optional<const std::exception&> exception, std::string_view message) {
    fmt::print(stderr, "Lua Exception: {}", message);

    return 0;
}

int main(int argc, const char** argv) {
    VES::Context ctx{};

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

    ctx.lua.open_libraries(sol::lib::base, sol::lib::package);
    ctx.lua.set_exception_handler(lua_exception_handler);
    ctx.RegisterLuaNatives();
    sol::load_result main_script = ctx.lua.load_file(fmt::format("{}/script/test.lua", ctx.datafod.string()));

    auto& world = ctx.world;
    {
        entt::entity terrain = world.create();
        ctx.scene["terrain"] = terrain;
        world.emplace<VES::Component::Name>(terrain, "terrain");
        world.emplace<VES::Component::Transform>(terrain, Vector3{-50.0f, 0.0f, -50.0f}, Vector3{0.0f, 0.0f, 0.0f}, Vector3{100.0f, 20.0f, 100.0f});
        world.emplace<VES::Component::Terrain>(terrain, terrain_image);
        world.emplace<VES::Component::Renderable>(terrain, &terrain_model);
        world.emplace<VES::Component::LuaBehavior>(terrain);
        VES::Map map{terrain};
        ctx.map = &map;

        Model teapot = LoadModel(fmt::format("{}/model/teapot.obj", ctx.datafod.string()).c_str());
        {
            entt::entity a = world.create();
            ctx.scene["a"] = a;
            world.emplace<VES::Component::Name>(a, "a");
            world.emplace<VES::Component::Transform>(a, Vector3{10.0f, 0.0f, 0.0f}, Vector3{0.0f, 0.0f, 0.0f}, Vector3{0.5f, 0.5f, 0.5f});
            world.emplace<VES::Component::Renderable>(a, &teapot, RED);
            world.emplace<VES::Component::SurfaceObject>(a);
            world.emplace<VES::Component::Behavior>(a,
                VES::Component::Behavior::CallbackMap{
                    {"update", [](VES::Context& ctx, entt::entity entity, float delta) {
                         VES::Component::Transform& transform = ctx.world.get<VES::Component::Transform>(entity);
                         transform.translation.x += (GetRandomValue(0, 255) / 255.0f) - 0.5f;
                         transform.translation.z += (GetRandomValue(0, 255) / 255.0f) - 0.5f;
                     }}});
            world.emplace<VES::Component::LuaBehavior>(a);

            entt::entity b = world.create();
            ctx.scene["b"] = b;
            world.emplace<VES::Component::Name>(b, "b");
            world.emplace<VES::Component::Transform>(b, Vector3{-10.0f, 0.0f, 0.0f}, Vector3{0.0f, 0.0f, 0.0f}, Vector3{0.5f, 0.5f, 0.5f});
            world.emplace<VES::Component::Renderable>(b, &teapot, GREEN);
            world.emplace<VES::Component::SurfaceObject>(b);
            world.emplace<VES::Component::Behavior>(b,
                VES::Component::Behavior::CallbackMap{
                    {"update", [](VES::Context& ctx, entt::entity entity, float delta) {
                         VES::Component::Transform& transform = ctx.world.get<VES::Component::Transform>(entity);
                         transform.translation.x += (ctx.camera.camera.target.x - transform.translation.x) * 0.1f * delta;
                         transform.translation.z += (ctx.camera.camera.target.z - transform.translation.z) * 0.1f * delta;
                     }}});
            world.emplace<VES::Component::LuaBehavior>(b);

            entt::entity c = world.create();
            ctx.scene["c"] = c;
            world.emplace<VES::Component::Name>(c, "c");
            world.emplace<VES::Component::Transform>(c, Vector3{0.0f, 0.0f, 10.0f}, Vector3{0.0f, 0.0f, 0.0f}, Vector3{0.5f, 0.5f, 0.5f});
            world.emplace<VES::Component::Renderable>(c, &teapot, BLUE);
            world.emplace<VES::Component::SurfaceObject>(c);
            world.emplace<VES::Component::LuaBehavior>(c);

            entt::entity d = world.create();
            ctx.scene["d"] = d;
            world.emplace<VES::Component::Name>(d, "d");
            world.emplace<VES::Component::Transform>(d, Vector3{20.0f, 0.0f, 20.0f}, Vector3{0.0f, 0.0f, 0.0f}, Vector3{5.0f, 5.0f, 5.0f});
            world.emplace<VES::Component::Renderable>(d, &teapot, YELLOW);
            world.emplace<VES::Component::SurfaceObject>(d);
            world.emplace<VES::Component::LuaBehavior>(d);
            world.emplace<VES::Component::UnboundedVerticalBlock>(d);
        }
    }

    main_script.call();

    while (!WindowShouldClose()) {
        float delta = GetFrameTime();
        ctx.camera.cam_target_destination.y = ctx.HeightAtPlanarWorldPos(Vector2{ctx.camera.camera.target.x, ctx.camera.camera.target.z});
        ctx.UpdateCamera(delta);
        ctx.Dispatch("update", delta);
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

    world.clear();

    return 0;
}