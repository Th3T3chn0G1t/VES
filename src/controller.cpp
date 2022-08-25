#include "include/ves.hpp"

using namespace VES;

static void Limit(float& val, float dest, float diff) {
    if (val < dest - diff) {
        val += diff;
    } else if (val > dest + diff) {
        val -= diff;
    }
}

void Context::UpdateCamera(float delta) {
    camera.target_destination.y = HeightAtPlanarWorldPos({camera.camera.target.x, camera.camera.target.z});

    glm::vec3& rot = camera.rotation;

    camera.forward = glm::vec3{static_cast<float>(std::sin(rot.x)), static_cast<float>(std::sin(rot.y)), static_cast<float>(std::sin(M_PI_2 - rot.x))};
    if (GetKeyHeld(KEY_Q) && camera.zoom - camera.zoom_speed * delta < camera.zoom_limits.y) {
        camera.zoom += camera.zoom_speed * delta;
    } else if (GetKeyHeld(KEY_E) && camera.zoom + camera.zoom_speed * delta > camera.zoom_limits.x) {
        camera.zoom -= camera.zoom_speed * delta;
    }

    if (GetKeyHeld(KEY_LEFT)) {
        rot.x += camera.turn_speed * delta;
        if (rot.x > M_PI * 2)
            rot.x -= M_PI * 2;
    } else if (GetKeyHeld(KEY_RIGHT)) {
        rot.x -= camera.turn_speed * delta;
        if (rot.x < 0)
            rot.x += M_PI * 2;
    }

    if (GetKeyHeld(KEY_UP) && rot.y - camera.turn_speed * delta > 0) {
        rot.y -= camera.turn_speed * delta;
    } else if (GetKeyHeld(KEY_DOWN) && rot.y + camera.turn_speed * delta < M_PI_2) {
        rot.y += camera.turn_speed * delta;
    }

    camera.left = glm::cross({camera.camera.up.x, camera.camera.up.y, camera.camera.up.z}, camera.forward);

    glm::vec2 planar_vec_to_cam{std::sin(rot.x), std::cos(rot.x)};
    glm::vec2 planar_left{std::sin(static_cast<float>(rot.x + M_PI_2)), std::cos(static_cast<float>(rot.x + M_PI_2))};
    glm::vec3& target_dest = camera.target_destination;
    glm::vec3& target_interpspeed = camera.target_destination_interp_speed;

    if (GetKeyHeld(KEY_W)) {
        camera.camera.target.x -= planar_vec_to_cam.x * camera.move_speed * delta;
        camera.camera.target.z -= planar_vec_to_cam.y * camera.move_speed * delta;
        camera.focus = NULL;
        camera.focused_name = NULL;
    } else if (GetKeyHeld(KEY_S)) {
        camera.camera.target.x += planar_vec_to_cam.x * camera.move_speed * delta;
        camera.camera.target.z += planar_vec_to_cam.y * camera.move_speed * delta;
        camera.focus = NULL;
        camera.focused_name = NULL;
    }

    if (GetKeyHeld(KEY_A)) {
        camera.camera.target.x -= planar_left.x * camera.move_speed * delta;
        camera.camera.target.z -= planar_left.y * camera.move_speed * delta;
        camera.focus = NULL;
        camera.focused_name = NULL;
    } else if (GetKeyHeld(KEY_D)) {
        camera.camera.target.x += planar_left.x * camera.move_speed * delta;
        camera.camera.target.z += planar_left.y * camera.move_speed * delta;
        camera.focus = NULL;
        camera.focused_name = NULL;
    }

    Limit(camera.camera.target.x, target_dest.x, target_interpspeed.x);
    Limit(camera.camera.target.y, target_dest.y, target_interpspeed.y);
    Limit(camera.camera.target.z, target_dest.z, target_interpspeed.z);

    if (camera.focus) {
        camera.camera.target = {
            camera.focus->x,
            camera.camera.target.y,
            camera.focus->z};
    }

    float y = camera.position.y + (std::sin(rot.y) * camera.zoom);
    glm::vec3& pos_dest = camera.position_destination;
    glm::vec3& pos_interpspeed = camera.position_destination_interp_speed;
    camera.camera.position = {
        camera.camera.target.x + (planar_vec_to_cam.x * camera.zoom),
        std::max(y, HeightAtPlanarWorldPos({camera.camera.position.x, camera.camera.position.z})),
        camera.camera.target.z + (planar_vec_to_cam.y * camera.zoom)};

    Limit(camera.camera.position.x, pos_dest.x, pos_interpspeed.x);
    Limit(camera.camera.position.y, pos_dest.y, pos_interpspeed.y);
    Limit(camera.camera.position.z, pos_dest.z, pos_interpspeed.z);

    ::UpdateCamera(&camera.camera);
}
