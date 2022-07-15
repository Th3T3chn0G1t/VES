#include "include/ves.hpp"

// Note to self: do not mix anything raylib with winapi (-> no global asio headers either :(.)

int main(int argc, const char** argv) {
    VES::Context::Config config{};
    config.datafod = "res";
    config.dim = {1280, 720};
    config.initial_map = "map/0.yaml";

    if (argc > 1) {
        config.datafod = argv[1];
    } else {
        if (!std::filesystem::is_directory(config.datafod)) {
            config.datafod = "../res";
        }
    }

    VES::Context ctx(config);

    ctx.ShowDialog("Why hello, welcome to VES! This is a test of whether text wrapping works properly! Hmmm, does that look right to you? If not, it is raylib's fault - blame it on the tools not the artist always ;^). Who doesn't love meme-y game libs that don't have neccesary features for making games *ahem* *ahem*.");

    while (!WindowShouldClose()) {
        ctx.UpdateUI();
        ctx.Update(GetFrameTime());
        BeginDrawing();
        {
            ClearBackground(BLACK);
            BeginMode3D(ctx.camera.camera);
            {
                ctx.DrawWorld();
                DrawCube(Vector3{ctx.camera.camera.target.x, ctx.camera.target_destination.y, ctx.camera.camera.target.z}, 1.0f, 1.0f, 1.0f, RED);
                if (ctx.camera.focus) {
                    float height = ctx.HeightAtPlanarWorldPos(Vector2{ctx.camera.focus->x, ctx.camera.focus->z});
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
            DrawText(fmt::format("Selected: {}", ctx.camera.focused_name ? *ctx.camera.focused_name : "unnamed").c_str(), 10, 10, ctx.ui_text_scale, RAYWHITE);
            ctx.DrawUI();
        }
        EndDrawing();
    }

    CloseWindow();

    ctx.world.clear();

    return 0;
}
