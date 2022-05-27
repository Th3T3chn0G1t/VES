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

    Shader shader = LoadShader("res/shader/base_lighting.vs", "resources/shader/lighting.fs");
    shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");
    int ambientLoc = GetShaderLocation(shader, "ambient");
    SetShaderValue(shader, ambientLoc, (float[4]){0.1f, 0.1f, 0.1f, 1.0f}, SHADER_UNIFORM_VEC4);
    Light lights[] = {CreateLight(LIGHT_POINT, (Vector3){-2, 1, -2}, Vector3Zero(), YELLOW, shader), CreateLight(LIGHT_POINT, (Vector3){2, 1, 2}, Vector3Zero(), RED, shader), CreateLight(LIGHT_POINT, (Vector3){-2, 1, 2}, Vector3Zero(), GREEN, shader), CreateLight(LIGHT_POINT, (Vector3){2, 1, -2}, Vector3Zero(), BLUE, shader)};

    entt::entity floor = ctx.world.create();
    ctx.world.emplace<TransformComponent>(floor, Vector3{0.0f, -1.5f, 0.0f}, Vector3{0.0f, 0.0f, 0.0f}, Vector3{100.0f, 1.0f, 100.0f});
    ctx.world.emplace<BoxComponent>(floor, GRAY);

    Model teapot = LoadModel("res/model/teapot.obj");
    teapot.materials[0].shader = shader;

    entt::entity a = ctx.world.create();
    ctx.world.emplace<TransformComponent>(a, Vector3{10.0f, 0.0f, 0.0f}, Vector3{0.0f, 0.0f, 0.0f}, Vector3{2.0f, 2.0f, 2.0f});
    ctx.world.emplace<RenderableComponent>(a, &teapot);
    entt::entity b = ctx.world.create();
    ctx.world.emplace<TransformComponent>(b, Vector3{-10.0f, 0.0f, 0.0f}, Vector3{0.0f, 0.0f, 0.0f}, Vector3{2.0f, 2.0f, 2.0f});
    ctx.world.emplace<RenderableComponent>(b, &teapot);
    entt::entity c = ctx.world.create();
    ctx.world.emplace<TransformComponent>(c, Vector3{0.0f, 0.0f, 10.0f}, Vector3{0.0f, 0.0f, 0.0f}, Vector3{2.0f, 2.0f, 2.0f});
    ctx.world.emplace<RenderableComponent>(c, &teapot);
    entt::entity d = ctx.world.create();
    ctx.world.emplace<TransformComponent>(d, Vector3{0.0f, 0.0f, -10.0f}, Vector3{0.0f, 0.0f, 0.0f}, Vector3{2.0f, 2.0f, 2.0f});
    ctx.world.emplace<RenderableComponent>(d, &teapot);

    while (!WindowShouldClose()) {
        float delta = GetFrameTime();
        UpdateCamera(ctx, delta);
        UpdateWorld(ctx, delta);
        for(size_t i = 0; i < sizeof(lights) / sizeof(lights[0]); ++i) UpdateLightValues(shader, lights[i]);
        SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], (float*) &ctx.camera.position, SHADER_UNIFORM_VEC3);
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