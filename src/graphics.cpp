#include "include/ves.hpp"

namespace VES {
    Model::Model(const std::filesystem::path& path) {
        // TODO: Raylib
        model = LoadModel(path.string().c_str());
    }

    Model::Model(const Texture& heightmap) {
        // TODO: Raylib
        Mesh mesh = GenMeshHeightmap(heightmap.image, {1.0f, 1.0f, 1.0f});
        model = LoadModelFromMesh(mesh);
        model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = heightmap.texture;
    }

    Bounds Model::GetBounds() const {
        // TODO: Raylib
        BoundingBox box = GetModelBoundingBox(model);
        return Bounds{
            {box.min.x, box.min.y, box.min.z},
            {box.max.x, box.max.y, box.max.z}};
    }

    Texture::Texture(const std::filesystem::path& path) {
        // TODO: Raylib
        image = LoadImage(path.string().c_str());
        texture = LoadTextureFromImage(image);

        extent.x = image.width;
        extent.y = image.height;
        data = (unsigned char*)image.data;
    }

    std::size_t Texture::GetPixelSize() const {
        // TODO: Raylib
        return GetPixelDataSize(1, 1, image.format);
    }

    Window::Window(const glm::ivec2& extent, const std::string& name) : extent(extent) {
        // TODO: Raylib
        InitWindow(extent.x, extent.y, name.c_str());
        SetTargetFPS(60);
    }

    Window::~Window() {
        // TODO: Raylib
        CloseWindow();
    }

    bool Window::GetShouldClose() {
        // TODO: Raylib
        return WindowShouldClose();
    }

    void Context::BeginFrame(const glm::vec4& clear) {
        // TODO: Raylib
        BeginDrawing();
        ClearBackground(
            Color{
                static_cast<unsigned char>(clear.x * 255),
                static_cast<unsigned char>(clear.y * 255),
                static_cast<unsigned char>(clear.z * 255),
                static_cast<unsigned char>(clear.w * 255)});
        BeginMode3D(camera.camera);
    }

    void Context::SubmitGeometry() {
        // TODO: Raylib
        EndMode3D();
    }

    void Context::EndFrame() {
        // TODO: Raylib
        EndDrawing();
    }

    void Context::DrawCube(const glm::vec3& pos, const glm::vec3& scale, const glm::vec4& color) {
        // TODO: Raylib
        ::DrawCube(
            Vector3{
                pos.x,
                pos.y,
                pos.z},
            scale.x,
            scale.y,
            scale.z,
            Color{
                static_cast<unsigned char>(color.x * 255),
                static_cast<unsigned char>(color.y * 255),
                static_cast<unsigned char>(color.z * 255),
                static_cast<unsigned char>(color.w * 255)});
    }

    int Context::GetTextWidth(const std::string& text, const int size) {
        // TODO: Raylib
        return MeasureText(text.c_str(), size);
    }

    void Context::DrawText(const std::string& text, const glm::vec2& pos, const int size, const glm::vec4& color) {
        // TODO: Raylib
        ::DrawText(
            text.c_str(),
            static_cast<int>(pos.x),
            static_cast<int>(pos.y),
            size,
            Color{
                static_cast<unsigned char>(color.x * 255),
                static_cast<unsigned char>(color.y * 255),
                static_cast<unsigned char>(color.z * 255),
                static_cast<unsigned char>(color.w * 255)});
    }

    void Context::DrawTexture(Texture& texture, const glm::vec2& pos, const glm::vec2& extent) {
        // TODO: Raylib
        DrawTextureQuad(texture.texture, Vector2{1.0f, 1.0f}, Vector2{0, 0}, Rectangle{pos.x, pos.y, extent.x, extent.y}, WHITE);
    }

    void Context::DrawModel(Model& model, const glm::vec3& translation, const glm::vec3& rotation, const glm::vec3& scale) {
        // TODO: Raylib
        DrawModelEx(
            model.model,
            {translation.x,
                translation.y,
                translation.z},
            {1.0f, 0.0f, 0.0f},
            rotation.x,
            {scale.x,
                scale.y,
                scale.z},
            WHITE);
    }
}
