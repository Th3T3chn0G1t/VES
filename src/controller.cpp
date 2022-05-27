#include "include/ves.hpp"

using namespace VES;

static void limit(float& val, float dest, float diff) {
    if (val < dest - diff)
        val += diff;
    else if (val > dest + diff)
        val -= diff;
}

void Context::UpdateCamera(float delta) {
    Vector3& rot = camera.cam_rotation;

    camera.cam_forward = Vector3{static_cast<float>(std::sin(rot.x)), static_cast<float>(std::sin(rot.y)), static_cast<float>(std::sin(M_PI_2 - rot.x))};
    if (IsKeyDown(KEY_Q) && camera.cam_zoom - camera.zoom_speed * delta < camera.zoom_limits.y) {
        camera.cam_zoom += camera.zoom_speed * delta;
    } else if (IsKeyDown(KEY_E) && camera.cam_zoom + camera.zoom_speed * delta > camera.zoom_limits.x) {
        camera.cam_zoom -= camera.zoom_speed * delta;
    }

    if (IsKeyDown(KEY_LEFT)) {
        rot.x += camera.turn_speed * delta;
        if (rot.x > M_PI * 2)
            rot.x -= M_PI * 2;
    } else if (IsKeyDown(KEY_RIGHT)) {
        rot.x -= camera.turn_speed * delta;
        if (rot.x < 0)
            rot.x += M_PI * 2;
    }
    if (IsKeyDown(KEY_UP) && rot.y - camera.turn_speed * delta > 0)
        rot.y -= camera.turn_speed * delta;
    else if (IsKeyDown(KEY_DOWN) && rot.y + camera.turn_speed * delta < M_PI_2)
        rot.y += camera.turn_speed * delta;

    camera.cam_left = Vector3CrossProduct(camera.camera.up, camera.cam_forward);

    Vector2 planar_vec_to_cam = Vector2{std::sin(rot.x), std::cos(rot.x)};
    Vector2 planar_left = Vector2{std::sin(static_cast<float>(rot.x + M_PI_2)), std::cos(static_cast<float>(rot.x + M_PI_2))};

    Vector3 &target = camera.camera.target, &target_dest = camera.cam_target_destination, &target_interpspeed = camera.cam_target_destination_interp_speed;

    if (IsKeyDown(KEY_W)) {
        target.x -= planar_vec_to_cam.x * camera.move_speed * delta;
        target.z -= planar_vec_to_cam.y * camera.move_speed * delta;
    } else if (IsKeyDown(KEY_S)) {
        target.x += planar_vec_to_cam.x * camera.move_speed * delta;
        target.z += planar_vec_to_cam.y * camera.move_speed * delta;
    }

    if (IsKeyDown(KEY_A)) {
        target.x -= planar_left.x * camera.move_speed * delta;
        target.z -= planar_left.y * camera.move_speed * delta;
    } else if (IsKeyDown(KEY_D)) {
        target.x += planar_left.x * camera.move_speed * delta;
        target.z += planar_left.y * camera.move_speed * delta;
    }

    limit(target.x, target_dest.x, target_interpspeed.x);
    limit(target.y, target_dest.y, target_interpspeed.y);
    limit(target.z, target_dest.z, target_interpspeed.z);

    float y = camera.cam_position.y + (std::sin(rot.y) * camera.cam_zoom);
    Vector3 &pos = camera.camera.position, &pos_dest = camera.cam_position_destination,
            &pos_interpspeed = camera.cam_position_destination_interp_speed;
    pos.x = target.x + (planar_vec_to_cam.x * camera.cam_zoom);
    pos.y = std::max(HeightAtPlanarWorldPos(Vector2{pos.x, pos.z}) + camera.cam_min_height, y);
    pos.z = target.z + (planar_vec_to_cam.y * camera.cam_zoom);

    limit(pos.x, pos_dest.x, pos_interpspeed.x);
    limit(pos.y, pos_dest.y, pos_interpspeed.y);
    limit(pos.z, pos_dest.z, pos_interpspeed.z);

    ::UpdateCamera(&camera.camera);
}
