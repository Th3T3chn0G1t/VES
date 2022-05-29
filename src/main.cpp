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

        YAML::Node map = YAML::LoadFile(fmt::format("{}/map/0.yaml", ctx.datafod.string()).c_str());
        for (const auto& object : map) {
            for (auto i = object.begin(); i != object.end(); ++i) {
                const auto& name = i->first.as<std::string>();
                ctx.scene[name] = ctx.world.create();
                world.emplace<VES::Component::Name>(ctx.scene[name], name);
                world.emplace<VES::Component::SurfaceObject>(ctx.scene[name]);
                world.emplace<VES::Component::LuaBehavior>(ctx.scene[name]);
                world.emplace<VES::Component::Selectable>(ctx.scene[name]);

                for (const auto& attribute : i->second) {
                    for (auto j = attribute.begin(); j != attribute.end(); ++j) {
                        if (std::string_view(j->first.as<std::string>()) == "transform") {
                            VES::Component::Transform transform;
                            for (const auto& aspect : j->second) {
                                for (auto k = aspect.begin(); k != aspect.end(); ++k) {
                                    const auto& vec = k->second.as<std::vector<float>>();
                                    if (std::string_view(k->first.as<std::string>()) == "translation") {
                                        transform.translation = Vector3{vec[0], vec[1], vec[2]};
                                    } else if (std::string_view(k->first.as<std::string>()) == "rotation") {
                                        transform.rotation = Vector3{vec[0], vec[1], vec[2]};
                                    } else if (std::string_view(k->first.as<std::string>()) == "scale") {
                                        transform.scale = Vector3{vec[0], vec[1], vec[2]};
                                    } else {
                                        fmt::print("Invalid transform component `{}`\n", k->first.as<std::string>());
                                        exit(1);
                                    }
                                }
                            }
                            world.emplace<VES::Component::Transform>(ctx.scene[name], transform);
                        } else if (std::string_view(j->first.as<std::string>()) == "renderable") {
                            VES::Component::Renderable renderable;
                            for (const auto& aspect : j->second) {
                                for (auto k = aspect.begin(); k != aspect.end(); ++k) {
                                    if (std::string_view(k->first.as<std::string>()) == "model") {
                                        Model& model = ctx.model_registry.GetOrLoad(fmt::format("{}/{}", ctx.datafod.string(), k->second.as<std::string>()));

                                        renderable.model = &model;
                                        world.emplace<VES::Component::UnboundedVerticalBlock>(ctx.scene[name], model);
                                        world.emplace<VES::Component::Blockable>(ctx.scene[name], model);
                                    } else if (std::string_view(k->first.as<std::string>()) == "tint") {
                                        const auto& vec = k->second.as<std::vector<unsigned char>>();
                                        renderable.tint = Color{vec[0], vec[1], vec[2], 255};
                                    } else {
                                        fmt::print("Invalid transform component `{}`\n", k->first.as<std::string>());
                                        exit(1);
                                    }
                                }
                            }
                            world.emplace<VES::Component::Renderable>(ctx.scene[name], renderable);
                        } else {
                            fmt::print("Unknown serialized component type `{}`", j->first.as<std::string>());
                            exit(1);
                        }
                    }
                }
            }
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
