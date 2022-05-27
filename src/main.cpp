#include "include/components.hpp"
#include "include/ves.hpp"

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

    entt::entity floor = ctx.world.create();
    ctx.world.emplace<TransformComponent>(floor, Vector3{0.0f, -1.5f, 0.0f}, Vector3{0.0f, 0.0f, 0.0f}, Vector3{100.0f, 1.0f, 100.0f});
    ctx.world.emplace<BoxComponent>(floor, GRAY);

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
        UpdateCamera(ctx, delta);
        UpdateWorld(ctx, delta);
        BeginDrawing();
        {
            ClearBackground(BLACK);
            BeginMode3D(ctx.camera);
            {
                DrawWorld(ctx, delta);
            }
            EndMode3D();
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}