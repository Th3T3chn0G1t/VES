#include "include/ves.hpp"

void UpdateCamera(VESContext& ctx, float delta) {
    auto& camera = ctx.camera;

    ctx.cam_forward = Vector3{static_cast<float>(std::sin(ctx.cam_rotation.x)), static_cast<float>(std::sin(ctx.cam_rotation.y)), static_cast<float>(std::sin(M_PI_2 - ctx.cam_rotation.x))};
    if (IsKeyDown(KEY_Q) && ctx.cam_zoom - ctx.zoom_speed * delta < ctx.zoom_limits.y) {
        ctx.cam_zoom += ctx.zoom_speed * delta;
    } else if (IsKeyDown(KEY_E) && ctx.cam_zoom + ctx.zoom_speed * delta > ctx.zoom_limits.x) {
        ctx.cam_zoom -= ctx.zoom_speed * delta;
    }

    if (IsKeyDown(KEY_LEFT)) {
        ctx.cam_rotation.x += ctx.turn_speed * delta;
        if (ctx.cam_rotation.x > M_PI * 2) {
            ctx.cam_rotation.x -= M_PI * 2;
        }
    } else if (IsKeyDown(KEY_RIGHT)) {
        ctx.cam_rotation.x -= ctx.turn_speed * delta;
        if (ctx.cam_rotation.x < 0) {
            ctx.cam_rotation.x += M_PI * 2;
        }
    }
    if (IsKeyDown(KEY_UP) && ctx.cam_rotation.y - ctx.turn_speed * delta > 0) {
        ctx.cam_rotation.y -= ctx.turn_speed * delta;
    } else if (IsKeyDown(KEY_DOWN) && ctx.cam_rotation.y + ctx.turn_speed * delta < M_PI_2) {
        ctx.cam_rotation.y += ctx.turn_speed * delta;
    }

    ctx.cam_left = Vector3CrossProduct(camera.up, ctx.cam_forward);

    Vector2 planar_vec_to_cam = Vector2{std::sin(ctx.cam_rotation.x), std::cos(ctx.cam_rotation.x)};
    Vector2 planar_left = Vector2{std::sin(static_cast<float>(ctx.cam_rotation.x + M_PI_2)), std::cos(static_cast<float>(ctx.cam_rotation.x + M_PI_2))};

    if (IsKeyDown(KEY_W)) {
        camera.target.x -= planar_vec_to_cam.x * ctx.move_speed * delta;
        camera.target.z -= planar_vec_to_cam.y * ctx.move_speed * delta;
    } else if (IsKeyDown(KEY_S)) {
        camera.target.x += planar_vec_to_cam.x * ctx.move_speed * delta;
        camera.target.z += planar_vec_to_cam.y * ctx.move_speed * delta;
    }

    if (IsKeyDown(KEY_A)) {
        camera.target.x -= planar_left.x * ctx.move_speed * delta;
        camera.target.z -= planar_left.y * ctx.move_speed * delta;
    } else if (IsKeyDown(KEY_D)) {
        camera.target.x += planar_left.x * ctx.move_speed * delta;
        camera.target.z += planar_left.y * ctx.move_speed * delta;
    }

    if (camera.target.x < ctx.cam_target_destination.x - ctx.cam_target_speed.x) {
        camera.target.x += ctx.cam_target_speed.x;
    } else if (camera.target.x > ctx.cam_target_destination.x + ctx.cam_target_speed.x) {
        camera.target.x -= ctx.cam_target_speed.x;
    }

    if (camera.target.y < ctx.cam_target_destination.y - ctx.cam_target_speed.y) {
        camera.target.y += ctx.cam_target_speed.y;
    } else if (camera.target.y > ctx.cam_target_destination.y + ctx.cam_target_speed.y) {
        camera.target.y -= ctx.cam_target_speed.y;
    }

    if (camera.target.z < ctx.cam_target_destination.z - ctx.cam_target_speed.z) {
        camera.target.z += ctx.cam_target_speed.z;
    } else if (camera.target.z > ctx.cam_target_destination.z + ctx.cam_target_speed.z) {
        camera.target.z -= ctx.cam_target_speed.z;
    }

    camera.position = Vector3{camera.target.x + (planar_vec_to_cam.x * ctx.cam_zoom), ctx.cam_position.y + (std::sin(ctx.cam_rotation.y) * ctx.cam_zoom), camera.target.z + (planar_vec_to_cam.y * ctx.cam_zoom)};

    UpdateCamera(&camera);
}
