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

    ctx.lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math, sol::lib::os);
    ctx.lua.set_exception_handler(lua_exception_handler);
    ctx.RegisterLuaNatives();
    sol::load_result main_script = ctx.lua.load_file(fmt::format("{}/script/test.lua", ctx.datafod.string()));

    auto& world = ctx.world;
    VES::Map map;
    ctx.map = &map;
    Model teapot = LoadModel(fmt::format("{}/model/teapot.obj", ctx.datafod.string()).c_str());
    {
        Image terrain_image = LoadImage(fmt::format("{}/texture/heightmap.png", ctx.datafod.string()).c_str());
        Texture2D terrain_texture = LoadTextureFromImage(terrain_image);
        Mesh terrain_mesh = GenMeshHeightmap(terrain_image, Vector3{1.0f, 1.0f, 1.0f});
        Model terrain_model = LoadModelFromMesh(terrain_mesh);
        terrain_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = terrain_texture;
        ctx.map->terrain_bounds = GetModelBoundingBox(terrain_model);

        ctx.map->terrain = world.create();
        ctx.scene["terrain"] = ctx.map->terrain;

        world.emplace<VES::Component::Name>(ctx.map->terrain, "terrain");
        VES::Component::Transform transform = {Vector3{-50.0f, 0.0f, -50.0f}, Vector3{0.0f, 0.0f, 0.0f}, Vector3{100.0f, 20.0f, 100.0f}};
        world.emplace<VES::Component::Transform>(ctx.map->terrain, transform);
        ctx.map->terrain_transform = &world.get<VES::Component::Transform>(ctx.map->terrain);
        world.emplace<VES::Component::Terrain>(ctx.map->terrain, terrain_image);
        world.emplace<VES::Component::Renderable>(ctx.map->terrain, &terrain_model);
        world.emplace<VES::Component::LuaBehavior>(ctx.map->terrain);

        ctx.map->width = static_cast<std::size_t>((ctx.map->terrain_bounds.max.x - ctx.map->terrain_bounds.min.x) * transform.scale.x);
        ctx.map->height = static_cast<std::size_t>((ctx.map->terrain_bounds.max.z - ctx.map->terrain_bounds.min.z) * transform.scale.z);
        ctx.map->grid.resize(ctx.map->width * ctx.map->height);
        std::fill(ctx.map->grid.begin(), ctx.map->grid.end(), VES::Cell{});

        {
            entt::entity a = world.create();
            ctx.scene["a"] = a;
            world.emplace<VES::Component::Name>(a, "a");
            world.emplace<VES::Component::Transform>(a, Vector3{10.0f, 0.0f, 0.0f}, Vector3{0.0f, 0.0f, 0.0f}, Vector3{0.5f, 0.5f, 0.5f});
            world.emplace<VES::Component::Renderable>(a, &teapot, RED);
            world.emplace<VES::Component::SurfaceObject>(a);
            world.emplace<VES::Component::LuaBehavior>(a);
            world.emplace<VES::Component::UnboundedVerticalBlock>(a, teapot);
            world.emplace<VES::Component::Blockable>(a, teapot);
            world.emplace<VES::Component::Selectable>(a);

            entt::entity b = world.create();
            ctx.scene["b"] = b;
            world.emplace<VES::Component::Name>(b, "b");
            world.emplace<VES::Component::Transform>(b, Vector3{-10.0f, 0.0f, 0.0f}, Vector3{0.0f, 0.0f, 0.0f}, Vector3{0.5f, 0.5f, 0.5f});
            world.emplace<VES::Component::Renderable>(b, &teapot, GREEN);
            world.emplace<VES::Component::SurfaceObject>(b);
            world.emplace<VES::Component::LuaBehavior>(b);
            world.emplace<VES::Component::Blockable>(b, teapot);
            world.emplace<VES::Component::Selectable>(b);

            entt::entity c = world.create();
            ctx.scene["c"] = c;
            world.emplace<VES::Component::Name>(c, "c");
            world.emplace<VES::Component::Transform>(c, Vector3{0.0f, 0.0f, 10.0f}, Vector3{0.0f, 0.0f, 0.0f}, Vector3{0.5f, 0.5f, 0.5f});
            world.emplace<VES::Component::Renderable>(c, &teapot, BLUE);
            world.emplace<VES::Component::SurfaceObject>(c);
            world.emplace<VES::Component::LuaBehavior>(c);
            world.emplace<VES::Component::Blockable>(c, teapot);
            world.emplace<VES::Component::Selectable>(c);

            entt::entity d = world.create();
            ctx.scene["d"] = d;
            world.emplace<VES::Component::Name>(d, "d");
            VES::Component::Transform transform{Vector3{20.0f, 10.0f, 20.0f}, Vector3{0.0f, 0.0f, 0.0f}, Vector3{5.0f, 5.0f, 5.0f}};
            world.emplace<VES::Component::Transform>(d, transform);
            world.emplace<VES::Component::Renderable>(d, &teapot, YELLOW);
            world.emplace<VES::Component::SurfaceObject>(d);
            world.emplace<VES::Component::LuaBehavior>(d);
            world.emplace<VES::Component::UnboundedVerticalBlock>(d, teapot);
            world.emplace<VES::Component::Blockable>(d, teapot);
            world.emplace<VES::Component::Selectable>(d);
        }
    }
    main_script.call();

    while (!WindowShouldClose()) {
        ctx.Update(GetFrameTime());
        BeginDrawing();
        {
            ClearBackground(BLACK);
            BeginMode3D(ctx.camera.camera);
            {
                ctx.DrawWorld();
                DrawCube(Vector3{ctx.camera.camera.target.x, ctx.camera.target_destination.y, ctx.camera.camera.target.z}, 1.0f, 1.0f, 1.0f, RED);
                if (ctx.camera.focus) {
                    float height = ctx.HeightAtPlanarWorldPos(Vector2{ctx.camera.focus->x, ctx.camera.focus->y});
                    DrawCircle3D(Vector3{ctx.camera.focus->x, height + ctx.camera.focus->y, ctx.camera.focus->z}, 2.5f, Vector3{1.0f, 0.0f, 0.0f}, 90.0f, RED);
                }

                for (std::size_t i = 0; i < ctx.map->grid.size(); ++i) {
                    if (ctx.map->grid[i].occupier.has_value()) {
                        DrawCube(
                            Vector3{
                                ctx.map->terrain_transform->translation.x + static_cast<float>(i % static_cast<size_t>(((ctx.map->terrain_bounds.max.x - ctx.map->terrain_bounds.min.x) * ctx.map->terrain_transform->scale.x))),
                                20.0f,
                                ctx.map->terrain_transform->translation.z + static_cast<float>(i / static_cast<size_t>(((ctx.map->terrain_bounds.max.x - ctx.map->terrain_bounds.min.x) * ctx.map->terrain_transform->scale.x)))},
                            1.0f, 1.0f, 1.0f, RED);
                    }
                }
            }
            EndMode3D();
            DrawText(fmt::format("Selected: {}", ctx.camera.focused_name ? *ctx.camera.focused_name : "unnamed").c_str(), 10, 10, 20, RAYWHITE);
        }
        EndDrawing();
    }

    CloseWindow();

    world.clear();

    return 0;
}
