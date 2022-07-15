#include "include/ves.hpp"

namespace VES {
    Texture2D Dialog::dialog_texture;

    void Context::ShowDialog(const std::string& text) {
        dialog.shown = true;
        dialog.text = text;
        dialog.current_character = 0;
    }

    Context::Context(const Context::Config& config) {
        datafod = config.datafod;
        screen_dim = config.dim;

        if (!std::filesystem::is_directory(datafod)) {
            fmt::print("Missing resfolder!");
            std::abort();
        }

        InitWindow(screen_dim.x, screen_dim.y, "VES");
        SetTargetFPS(60);
        SetCameraMode(camera.camera, CAMERA_CUSTOM);

        lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math, sol::lib::os);
        RegisterLuaNatives();

        Dialog::dialog_texture = LoadTexture(fmt::format("{}/texture/dialog_bg.png", datafod.string()).c_str());

        std::vector<sol::load_result> scripts = LoadMap(config.initial_map);
        for (auto& script : scripts) {
            script.call();
        }

        dialog_width = screen_dim.x * 0.75f;
        dialog_height = screen_dim.y * 0.3f;
        dialog_x = (screen_dim.x / 2.0f) - (dialog_width / 2.0f);
        dialog_y = screen_dim.y - (dialog_height * 1.1f);
    }

    std::vector<sol::load_result> Context::LoadMap(const std::filesystem::path& path) {
        std::vector<sol::load_result> scripts;

        map = std::make_unique<Map>();
        YAML::Node map_file = YAML::LoadFile(fmt::format("{}/{}", datafod.string(), path.string()).c_str());
        for (const auto& object : map_file) {
            for (auto i = object.begin(); i != object.end(); ++i) {
                const auto& name = i->first.as<std::string>();
                if (name == "script") {
                    for (const auto& attribute : i->second) {
                        scripts.push_back(lua.load_file(fmt::format("{}/{}", datafod.string(), attribute.as<std::string>())));
                    }
                    continue;
                } else if (name == "heightmap") {
                    Image terrain_image = LoadImage(fmt::format("{}/{}", datafod.string(), i->second.as<std::string>()).c_str());
                    Texture2D terrain_texture = LoadTextureFromImage(terrain_image);
                    Mesh terrain_mesh = GenMeshHeightmap(terrain_image, Vector3{1.0f, 1.0f, 1.0f});
                    Model terrain_model = LoadModelFromMesh(terrain_mesh);
                    terrain_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = terrain_texture;
                    map->terrain_bounds = GetModelBoundingBox(terrain_model);

                    map->terrain = world.create();
                    scene["terrain"] = map->terrain;

                    world.emplace<VES::Component::Name>(map->terrain, "terrain");
                    VES::Component::Transform transform = {Vector3{-250.0f, -10.0f, -250.0f}, Vector3{0.0f, 0.0f, 0.0f}, Vector3{500.0f, 20.0f, 500.0f}};
                    world.emplace<VES::Component::Transform>(map->terrain, transform);
                    map->terrain_transform = &world.get<VES::Component::Transform>(map->terrain);
                    world.emplace<VES::Component::Terrain>(map->terrain, terrain_image);
                    world.emplace<VES::Component::Renderable>(map->terrain, &terrain_model);
                    world.emplace<VES::Component::LuaBehavior>(map->terrain);

                    map->width = static_cast<std::size_t>((map->terrain_bounds.max.x - map->terrain_bounds.min.x) * transform.scale.x);
                    map->height = static_cast<std::size_t>((map->terrain_bounds.max.z - map->terrain_bounds.min.z) * transform.scale.z);
                    map->grid.resize(map->width * map->height);
                    std::fill(map->grid.begin(), map->grid.end(), VES::Cell{});

                    continue;
                }

                scene[name] = world.create();
                world.emplace<VES::Component::Name>(scene[name], name);
                world.emplace<VES::Component::SurfaceObject>(scene[name]);
                world.emplace<VES::Component::LuaBehavior>(scene[name]);
                world.emplace<VES::Component::Selectable>(scene[name]);

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
                            world.emplace<VES::Component::Transform>(scene[name], transform);
                        } else if (std::string_view(j->first.as<std::string>()) == "renderable") {
                            VES::Component::Renderable renderable;
                            for (const auto& aspect : j->second) {
                                for (auto k = aspect.begin(); k != aspect.end(); ++k) {
                                    if (std::string_view(k->first.as<std::string>()) == "model") {
                                        Model& model = model_registry.GetOrLoad(fmt::format("{}/{}", datafod.string(), k->second.as<std::string>()));

                                        renderable.model = &model;
                                        world.emplace<VES::Component::UnboundedVerticalBlock>(scene[name], model);
                                        world.emplace<VES::Component::Blockable>(scene[name], model);
                                    } else if (std::string_view(k->first.as<std::string>()) == "tint") {
                                        const auto& vec = k->second.as<std::vector<int>>();
                                        renderable.tint = Color{static_cast<unsigned char>(vec[0]), static_cast<unsigned char>(vec[1]), static_cast<unsigned char>(vec[2]), 255};
                                    } else {
                                        fmt::print("Invalid transform component `{}`\n", k->first.as<std::string>());
                                        exit(1);
                                    }
                                }
                            }
                            world.emplace<VES::Component::Renderable>(scene[name], renderable);
                        } else {
                            fmt::print("Unknown serialized component type `{}`", j->first.as<std::string>());
                            exit(1);
                        }
                    }
                }
            }
        }
        return scripts;
    }

    void Context::UpdateUI() {
        mouse_blocked = false;

        if (dialog.shown && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 pos = GetMousePosition();
            if (pos.x > dialog_x && pos.x < dialog_x + dialog_width) {
                if (pos.y > dialog_y && pos.y < dialog_y + dialog_height) {
                    mouse_blocked = true;
                    dialog.shown = false;
                }
            }
        }
    }

    void Context::DrawUI() {
        if (dialog.shown) {
            DrawTextureQuad(VES::Dialog::dialog_texture, Vector2{1.0f, 1.0f}, Vector2{0, 0}, Rectangle{dialog_x, dialog_y, dialog_width, dialog_height}, WHITE);

            float y = dialog_y + dialog_height * 0.1f;

            std::size_t last_line_end = 0;
            std::size_t current_char_whole = 0;
            while (current_char_whole < static_cast<std::size_t>(dialog.current_character)) {
                float width = 0.0f;
                std::string substr = "unnamed";
                while (width < dialog_width * 0.9f) {
                    substr = dialog.text.substr(last_line_end, current_char_whole - last_line_end);

                    width = MeasureText(substr.c_str(), ui_text_scale);

                    if (++current_char_whole >= static_cast<std::size_t>(dialog.current_character)) {
                        break;
                    }
                }
                substr = dialog.text.substr(last_line_end, current_char_whole - last_line_end);
                DrawText(substr.c_str(), dialog_x + dialog_height * 0.1f, y, ui_text_scale, BLACK);
                last_line_end = current_char_whole;

                y += ui_text_scale;
            }
            if (dialog.current_character < dialog.text.size()) {
                dialog.current_character += VES::Dialog::speed;
            }
        }
    }
}
