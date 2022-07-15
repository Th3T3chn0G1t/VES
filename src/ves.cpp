#include "include/ves.hpp"

namespace VES {
    float Context::GetFrameDelta() {
        // TODO: Raylib
        return GetFrameTime();
    }

    bool Context::GetKeyHeld(const Key key) {
        // TODO: Raylib
        return IsKeyDown(key);
    }

    bool Context::GetButtonHeld(const Button button) {
        // TODO: Raylib
        return IsMouseButtonPressed(button);
    }

    glm::vec2 Context::GetMousePosition() {
        // TODO: Raylib
        Vector2 pos = ::GetMousePosition();
        return {pos.x, pos.y};
    }

    void Context::ShowDialog(const std::string& text) {
        dialog.shown = true;
        dialog.text = text;
        dialog.current_character = 0;
    }

    Context::Context(const Context::Config& config) : window(config.dim, config.name) {
        datafod = config.datafod;

        if (!std::filesystem::is_directory(datafod)) {
            fmt::print("Missing resfolder!");
            std::abort();
        }

        // TODO: Raylib
        SetCameraMode(camera.camera, CAMERA_CUSTOM);

        lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math, sol::lib::os);
        RegisterLuaNatives();

        dialog.dialog_texture = &texture_registry.GetOrLoad(fmt::format("{}/texture/dialog_bg.png", datafod.string()));

        std::vector<sol::load_result> scripts = LoadMap(config.initial_map);
        for (auto& script : scripts) {
            script.call();
        }

        dialog_width = window.extent.x * 0.75f;
        dialog_height = window.extent.y * 0.3f;
        dialog_x = (window.extent.x / 2.0f) - (dialog_width / 2.0f);
        dialog_y = window.extent.y - (dialog_height * 1.1f);
    }

    Context::~Context() {
        world.clear();
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
                    const Texture& heightmap = texture_registry.GetOrLoad(fmt::format("{}/{}", datafod.string(), i->second.as<std::string>()));
                    map->terrain_model = Model(heightmap);
                    map->terrain_bounds = map->terrain_model.GetBounds();
                    map->terrain = world.create();
                    scene["terrain"] = map->terrain;

                    world.emplace<Component::Name>(map->terrain, "terrain");
                    Component::Transform transform = {{-250.0f, -10.0f, -250.0f}, {0.0f, 0.0f, 0.0f}, {500.0f, 20.0f, 500.0f}};
                    world.emplace<Component::Transform>(map->terrain, transform);
                    map->terrain_transform = &world.get<Component::Transform>(map->terrain);
                    world.emplace<Component::Terrain>(map->terrain, heightmap);
                    world.emplace<Component::Renderable>(map->terrain, &map->terrain_model);
                    world.emplace<Component::LuaBehavior>(map->terrain);

                    map->width = static_cast<std::size_t>((map->terrain_bounds.max.x - map->terrain_bounds.min.x) * transform.scale.x);
                    map->height = static_cast<std::size_t>((map->terrain_bounds.max.z - map->terrain_bounds.min.z) * transform.scale.z);
                    map->grid.resize(map->width * map->height);
                    std::fill(map->grid.begin(), map->grid.end(), Cell{});

                    continue;
                }

                scene[name] = world.create();
                world.emplace<Component::Name>(scene[name], name);
                world.emplace<Component::SurfaceObject>(scene[name]);
                world.emplace<Component::LuaBehavior>(scene[name]);
                world.emplace<Component::Selectable>(scene[name]);

                for (const auto& attribute : i->second) {
                    for (auto j = attribute.begin(); j != attribute.end(); ++j) {
                        if (std::string_view(j->first.as<std::string>()) == "transform") {
                            Component::Transform transform;
                            for (const auto& aspect : j->second) {
                                for (auto k = aspect.begin(); k != aspect.end(); ++k) {
                                    const auto& vec = k->second.as<std::vector<float>>();
                                    if (std::string_view(k->first.as<std::string>()) == "translation") {
                                        transform.translation = glm::vec3{vec[0], vec[1], vec[2]};
                                    } else if (std::string_view(k->first.as<std::string>()) == "rotation") {
                                        transform.rotation = glm::vec3{vec[0], vec[1], vec[2]};
                                    } else if (std::string_view(k->first.as<std::string>()) == "scale") {
                                        transform.scale = glm::vec3{vec[0], vec[1], vec[2]};
                                    } else {
                                        fmt::print("Invalid transform component `{}`\n", k->first.as<std::string>());
                                        exit(1);
                                    }
                                }
                            }
                            world.emplace<Component::Transform>(scene[name], transform);
                        } else if (std::string_view(j->first.as<std::string>()) == "renderable") {
                            Component::Renderable renderable;
                            for (const auto& aspect : j->second) {
                                for (auto k = aspect.begin(); k != aspect.end(); ++k) {
                                    if (std::string_view(k->first.as<std::string>()) == "model") {
                                        Model& model = model_registry.GetOrLoad(fmt::format("{}/{}", datafod.string(), k->second.as<std::string>()));

                                        renderable.model = &model;
                                        world.emplace<Component::UnboundedVerticalBlock>(scene[name], model);
                                        world.emplace<Component::Blockable>(scene[name], model);
                                    } else {
                                        fmt::print("Invalid transform component `{}`\n", k->first.as<std::string>());
                                        exit(1);
                                    }
                                }
                            }
                            world.emplace<Component::Renderable>(scene[name], renderable);
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

        if (dialog.shown && GetButtonHeld(MOUSE_BUTTON_LEFT)) {
            glm::vec2 pos = GetMousePosition();
            if (pos.x > dialog_x && pos.x < dialog_x + dialog_width) {
                if (pos.y > dialog_y && pos.y < dialog_y + dialog_height) {
                    mouse_blocked = true;
                    dialog.shown = false;
                }
            }
        }
    }

    void Context::DrawUI() {
        DrawText(
            fmt::format("Selected: {}", camera.focused_name ? *camera.focused_name : "unnamed"),
            {10.0f, 10.0f},
            ui_text_scale,
            {1.0f, 1.0f, 1.0f, 1.0f});

        if (dialog.shown) {
            DrawTexture(*dialog.dialog_texture, {dialog_x, dialog_y}, {dialog_width, dialog_height});

            float y = dialog_y + dialog_height * 0.1f;

            std::size_t last_line_end = 0;
            std::size_t current_char_whole = 0;
            while (current_char_whole < static_cast<std::size_t>(dialog.current_character)) {
                float width = 0.0f;
                std::string substr = "unnamed";
                while (width < dialog_width * 0.9f) {
                    substr = dialog.text.substr(last_line_end, current_char_whole - last_line_end);

                    width = GetTextWidth(substr, ui_text_scale);

                    if (++current_char_whole >= static_cast<std::size_t>(dialog.current_character)) {
                        break;
                    }
                }
                substr = dialog.text.substr(last_line_end, current_char_whole - last_line_end);
                DrawText(substr, {dialog_x + dialog_height * 0.1f, y}, ui_text_scale, {0.0f, 0.0f, 0.0f, 1.0f});
                last_line_end = current_char_whole;

                y += ui_text_scale;
            }
            if (dialog.current_character < dialog.text.size()) {
                dialog.current_character += Dialog::speed;
            }
        }
    }

    bool Context::CheckMousePick(const Bounds& bounds) {
        // TODO: Raylib
        BoundingBox real_bounds = {
            {bounds.min.x, bounds.min.y, bounds.min.z},
            {bounds.max.x, bounds.max.y, bounds.max.z}};
        return GetRayCollisionBox(GetMouseRay(::GetMousePosition(), camera.camera), real_bounds).hit;
    }
}
