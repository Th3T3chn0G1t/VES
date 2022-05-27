#include "include/ves.hpp"

using namespace VES;

void Context::UpdateCamera(float delta) {
    camera.cam_forward = Vector3{static_cast<float>(std::sin(camera.cam_rotation.x)), static_cast<float>(std::sin(camera.cam_rotation.y)), static_cast<float>(std::sin(M_PI_2 - camera.cam_rotation.x))};
    if (IsKeyDown(KEY_Q) && camera.cam_zoom - camera.zoom_speed * delta < camera.zoom_limits.y) {
        camera.cam_zoom += camera.zoom_speed * delta;
    } else if (IsKeyDown(KEY_E) && camera.cam_zoom + camera.zoom_speed * delta > camera.zoom_limits.x) {
        camera.cam_zoom -= camera.zoom_speed * delta;
    }

    if (IsKeyDown(KEY_LEFT)) {
        camera.cam_rotation.x += camera.turn_speed * delta;
        if (camera.cam_rotation.x > M_PI * 2) {
            camera.cam_rotation.x -= M_PI * 2;
        }
    } else if (IsKeyDown(KEY_RIGHT)) {
        camera.cam_rotation.x -= camera.turn_speed * delta;
        if (camera.cam_rotation.x < 0) {
            camera.cam_rotation.x += M_PI * 2;
        }
    }
    if (IsKeyDown(KEY_UP) && camera.cam_rotation.y - camera.turn_speed * delta > 0) {
        camera.cam_rotation.y -= camera.turn_speed * delta;
    } else if (IsKeyDown(KEY_DOWN) && camera.cam_rotation.y + camera.turn_speed * delta < M_PI_2) {
        camera.cam_rotation.y += camera.turn_speed * delta;
    }

    camera.cam_left = Vector3CrossProduct(camera.camera.up, camera.cam_forward);

    Vector2 planar_vec_to_cam = Vector2{std::sin(camera.cam_rotation.x), std::cos(camera.cam_rotation.x)};
    Vector2 planar_left = Vector2{std::sin(static_cast<float>(camera.cam_rotation.x + M_PI_2)), std::cos(static_cast<float>(camera.cam_rotation.x + M_PI_2))};

    if (IsKeyDown(KEY_W)) {
        camera.camera.target.x -= planar_vec_to_cam.x * camera.move_speed * delta;
        camera.camera.target.z -= planar_vec_to_cam.y * camera.move_speed * delta;
    } else if (IsKeyDown(KEY_S)) {
        camera.camera.target.x += planar_vec_to_cam.x * camera.move_speed * delta;
        camera.camera.target.z += planar_vec_to_cam.y * camera.move_speed * delta;
    }

    if (IsKeyDown(KEY_A)) {
        camera.camera.target.x -= planar_left.x * camera.move_speed * delta;
        camera.camera.target.z -= planar_left.y * camera.move_speed * delta;
    } else if (IsKeyDown(KEY_D)) {
        camera.camera.target.x += planar_left.x * camera.move_speed * delta;
        camera.camera.target.z += planar_left.y * camera.move_speed * delta;
    }

    if (camera.camera.target.x < camera.cam_target_destination.x - camera.cam_target_destination_interp_speed.x) {
        camera.camera.target.x += camera.cam_target_destination_interp_speed.x;
    } else if (camera.camera.target.x > camera.cam_target_destination.x + camera.cam_target_destination_interp_speed.x) {
        camera.camera.target.x -= camera.cam_target_destination_interp_speed.x;
    }

    if (camera.camera.target.y < camera.cam_target_destination.y - camera.cam_target_destination_interp_speed.y) {
        camera.camera.target.y += camera.cam_target_destination_interp_speed.y;
    } else if (camera.camera.target.y > camera.cam_target_destination.y + camera.cam_target_destination_interp_speed.y) {
        camera.camera.target.y -= camera.cam_target_destination_interp_speed.y;
    }

    if (camera.camera.target.z < camera.cam_target_destination.z - camera.cam_target_destination_interp_speed.z) {
        camera.camera.target.z += camera.cam_target_destination_interp_speed.z;
    } else if (camera.camera.target.z > camera.cam_target_destination.z + camera.cam_target_destination_interp_speed.z) {
        camera.camera.target.z -= camera.cam_target_destination_interp_speed.z;
    }

    float y = camera.cam_position.y + (std::sin(camera.cam_rotation.y) * camera.cam_zoom);
    camera.camera.position = Vector3{camera.camera.target.x + (planar_vec_to_cam.x * camera.cam_zoom), std::max(HeightAtPlanarWorldPos(Vector2{camera.camera.position.x, camera.camera.position.z}) + camera.cam_min_height, y), camera.camera.target.z + (planar_vec_to_cam.y * camera.cam_zoom)};

    if (camera.camera.position.x < camera.cam_position_destination.x - camera.cam_position_destination_interp_speed.x) {
        camera.camera.position.x += camera.cam_position_destination_interp_speed.x;
    } else if (camera.camera.position.x > camera.cam_position_destination.x + camera.cam_position_destination_interp_speed.x) {
        camera.camera.position.x -= camera.cam_position_destination_interp_speed.x;
    }

    if (camera.camera.position.y < camera.cam_position_destination.y - camera.cam_position_destination_interp_speed.y) {
        camera.camera.position.y += camera.cam_position_destination_interp_speed.y;
    } else if (camera.camera.position.y > camera.cam_position_destination.y + camera.cam_position_destination_interp_speed.y) {
        camera.camera.position.y -= camera.cam_position_destination_interp_speed.y;
    }

    if (camera.camera.position.z < camera.cam_position_destination.z - camera.cam_position_destination_interp_speed.z) {
        camera.camera.position.z += camera.cam_position_destination_interp_speed.z;
    } else if (camera.camera.position.z > camera.cam_position_destination.z + camera.cam_position_destination_interp_speed.z) {
        camera.camera.position.z -= camera.cam_position_destination_interp_speed.z;
    }

    ::UpdateCamera(&camera.camera);
}
