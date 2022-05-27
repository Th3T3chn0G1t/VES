#include "include/ves.hpp"

using namespace VES;

void Context::UpdateCamera(float delta) {
    auto& camera = this->camera;

    this->cam_forward = Vector3{static_cast<float>(std::sin(this->cam_rotation.x)), static_cast<float>(std::sin(this->cam_rotation.y)), static_cast<float>(std::sin(M_PI_2 - this->cam_rotation.x))};
    if (IsKeyDown(KEY_Q) && this->cam_zoom - this->zoom_speed * delta < this->zoom_limits.y) {
        this->cam_zoom += this->zoom_speed * delta;
    } else if (IsKeyDown(KEY_E) && this->cam_zoom + this->zoom_speed * delta > this->zoom_limits.x) {
        this->cam_zoom -= this->zoom_speed * delta;
    }

    if (IsKeyDown(KEY_LEFT)) {
        this->cam_rotation.x += this->turn_speed * delta;
        if (this->cam_rotation.x > M_PI * 2) {
            this->cam_rotation.x -= M_PI * 2;
        }
    } else if (IsKeyDown(KEY_RIGHT)) {
        this->cam_rotation.x -= this->turn_speed * delta;
        if (this->cam_rotation.x < 0) {
            this->cam_rotation.x += M_PI * 2;
        }
    }
    if (IsKeyDown(KEY_UP) && this->cam_rotation.y - this->turn_speed * delta > 0) {
        this->cam_rotation.y -= this->turn_speed * delta;
    } else if (IsKeyDown(KEY_DOWN) && this->cam_rotation.y + this->turn_speed * delta < M_PI_2) {
        this->cam_rotation.y += this->turn_speed * delta;
    }

    this->cam_left = Vector3CrossProduct(camera.up, this->cam_forward);

    Vector2 planar_vec_to_cam = Vector2{std::sin(this->cam_rotation.x), std::cos(this->cam_rotation.x)};
    Vector2 planar_left = Vector2{std::sin(static_cast<float>(this->cam_rotation.x + M_PI_2)), std::cos(static_cast<float>(this->cam_rotation.x + M_PI_2))};

    if (IsKeyDown(KEY_W)) {
        camera.target.x -= planar_vec_to_cam.x * this->move_speed * delta;
        camera.target.z -= planar_vec_to_cam.y * this->move_speed * delta;
    } else if (IsKeyDown(KEY_S)) {
        camera.target.x += planar_vec_to_cam.x * this->move_speed * delta;
        camera.target.z += planar_vec_to_cam.y * this->move_speed * delta;
    }

    if (IsKeyDown(KEY_A)) {
        camera.target.x -= planar_left.x * this->move_speed * delta;
        camera.target.z -= planar_left.y * this->move_speed * delta;
    } else if (IsKeyDown(KEY_D)) {
        camera.target.x += planar_left.x * this->move_speed * delta;
        camera.target.z += planar_left.y * this->move_speed * delta;
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

    camera.position = Vector3{camera.target.x + (planar_vec_to_cam.x * this->cam_zoom), std::max(this->HeightAtPlanarWorldPos(Vector2{this->camera.position.x, this->camera.position.z}) + cam_min_height, this->cam_position.y + (std::sin(this->cam_rotation.y) * this->cam_zoom)), camera.target.z + (planar_vec_to_cam.y * this->cam_zoom)};

    ::UpdateCamera(&camera);
}
