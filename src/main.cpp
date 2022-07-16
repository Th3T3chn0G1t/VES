#include "include/ves.hpp"

// Note to self: do not mix anything raylib with winapi (-> no global asio headers either :(.)

int main(int argc, const char** argv) {
    VES::Context::Config config("ves.yaml");

    if (argc > 1) {
        config.datafod = argv[1];
    } else {
        if (!std::filesystem::is_directory(config.datafod)) {
            config.datafod = std::filesystem::path("../") / config.datafod;
        }
    }

    VES::Context ctx(config);

    ctx.ShowDialog("Why hello, welcome to VES! This is a test of whether text wrapping works properly! Hmmm, does that look right to you? If not, it is raylib's fault - blame it on the tools not the artist always ;^). Who doesn't love meme-y game libs that don't have neccesary features for making games *ahem* *ahem*.");

    while (!ctx.window.GetShouldClose()) {
        ctx.UpdateUI();
        ctx.Update(ctx.GetFrameDelta());

        ctx.BeginFrame({0.0f, 0.0f, 0.0f, 1.0f});
        {
            ctx.DrawWorld();
            ctx.DrawCube(
                {ctx.camera.camera.target.x,
                    ctx.camera.target_destination.y,
                    ctx.camera.camera.target.z},
                {1.0f, 1.0f, 1.0f},
                {1.0f, 0.0f, 0.0f, 1.0f});

            for (std::size_t i = 0; i < ctx.map->grid.size(); ++i) {
                if (ctx.map->grid[i].occupier.has_value()) {
                    ctx.DrawCube(
                        {ctx.map->terrain_transform->translation.x + static_cast<float>(i % static_cast<std::size_t>(((ctx.map->terrain_bounds.max.x - ctx.map->terrain_bounds.min.x) * ctx.map->terrain_transform->scale.x))),
                            20.0f,
                            ctx.map->terrain_transform->translation.z + static_cast<float>(i / static_cast<std::size_t>(((ctx.map->terrain_bounds.max.x - ctx.map->terrain_bounds.min.x) * ctx.map->terrain_transform->scale.x)))},
                        {1.0f, 1.0f, 1.0f},
                        {1.0f, 0.0f, 0.0f, 1.0f});
                }
            }
            ctx.SubmitGeometry();

            ctx.DrawUI();
        }
        ctx.EndFrame();
    }

    return 0;
}
