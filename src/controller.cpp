#include "include/ves.hpp"

using namespace VES;

void Context::UpdateCamera(float delta) {
    auto& camera = this->camera.camera;

    this->camera.cam_forward = Vector3{static_cast<float>(std::sin(this->camera.cam_rotation.x)), static_cast<float>(std::sin(this->camera.cam_rotation.y)), static_cast<float>(std::sin(M_PI_2 - this->camera.cam_rotation.x))};
    if (IsKeyDown(KEY_Q) && this->camera.cam_zoom - this->camera.zoom_speed * delta < this->camera.zoom_limits.y) {
        this->camera.cam_zoom += this->camera.zoom_speed * delta;
    } else if (IsKeyDown(KEY_E) && this->camera.cam_zoom + this->camera.zoom_speed * delta > this->camera.zoom_limits.x) {
        this->camera.cam_zoom -= this->camera.zoom_speed * delta;
    }

    if (IsKeyDown(KEY_LEFT)) {
        this->camera.cam_rotation.x += this->camera.turn_speed * delta;
        if (this->camera.cam_rotation.x > M_PI * 2) {
            this->camera.cam_rotation.x -= M_PI * 2;
        }
    } else if (IsKeyDown(KEY_RIGHT)) {
        this->camera.cam_rotation.x -= this->camera.turn_speed * delta;
        if (this->camera.cam_rotation.x < 0) {
            this->camera.cam_rotation.x += M_PI * 2;
        }
    }
    if (IsKeyDown(KEY_UP) && this->camera.cam_rotation.y - this->camera.turn_speed * delta > 0) {
        this->camera.cam_rotation.y -= this->camera.turn_speed * delta;
    } else if (IsKeyDown(KEY_DOWN) && this->camera.cam_rotation.y + this->camera.turn_speed * delta < M_PI_2) {
        this->camera.cam_rotation.y += this->camera.turn_speed * delta;
    }

    this->camera.cam_left = Vector3CrossProduct(camera.up, this->camera.cam_forward);

    Vector2 planar_vec_to_cam = Vector2{std::sin(this->camera.cam_rotation.x), std::cos(this->camera.cam_rotation.x)};
    Vector2 planar_left = Vector2{std::sin(static_cast<float>(this->camera.cam_rotation.x + M_PI_2)), std::cos(static_cast<float>(this->camera.cam_rotation.x + M_PI_2))};

    if (IsKeyDown(KEY_W)) {
        camera.target.x -= planar_vec_to_cam.x * this->camera.move_speed * delta;
        camera.target.z -= planar_vec_to_cam.y * this->camera.move_speed * delta;
    } else if (IsKeyDown(KEY_S)) {
        camera.target.x += planar_vec_to_cam.x * this->camera.move_speed * delta;
        camera.target.z += planar_vec_to_cam.y * this->camera.move_speed * delta;
    }

    if (IsKeyDown(KEY_A)) {
        camera.target.x -= planar_left.x * this->camera.move_speed * delta;
        camera.target.z -= planar_left.y * this->camera.move_speed * delta;
    } else if (IsKeyDown(KEY_D)) {
        camera.target.x += planar_left.x * this->camera.move_speed * delta;
        camera.target.z += planar_left.y * this->camera.move_speed * delta;
    }

    if (camera.target.x < this->camera.cam_target_destination.x - this->camera.cam_target_speed.x) {
        camera.target.x += this->camera.cam_target_speed.x;
    } else if (camera.target.x > this->camera.cam_target_destination.x + this->camera.cam_target_speed.x) {
        camera.target.x -= this->camera.cam_target_speed.x;
    }

    if (camera.target.y < this->camera.cam_target_destination.y - this->camera.cam_target_speed.y) {
        camera.target.y += this->camera.cam_target_speed.y;
    } else if (camera.target.y > this->camera.cam_target_destination.y + this->camera.cam_target_speed.y) {
        camera.target.y -= this->camera.cam_target_speed.y;
    }

    if (camera.target.z < this->camera.cam_target_destination.z - this->camera.cam_target_speed.z) {
        camera.target.z += this->camera.cam_target_speed.z;
    } else if (camera.target.z > this->camera.cam_target_destination.z + this->camera.cam_target_speed.z) {
        camera.target.z -= this->camera.cam_target_speed.z;
    }

    camera.position = Vector3{camera.target.x + (planar_vec_to_cam.x * this->camera.cam_zoom), std::max(this->HeightAtPlanarWorldPos(Vector2{this->camera.camera.position.x, this->camera.camera.position.z}) + this->camera.cam_min_height, this->camera.cam_position.y + (std::sin(this->camera.cam_rotation.y) * this->camera.cam_zoom)), camera.target.z + (planar_vec_to_cam.y * this->camera.cam_zoom)};

    ::UpdateCamera(&camera);
}
