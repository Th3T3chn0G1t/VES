#include "include/ves.hpp"

namespace VES {
    void Context::PlanarMove(entt::entity entity, Vector2 amount) {
        Component::Transform& transform =  world.get<Component::Transform>(entity);
        transform.translation.x += amount.x;
        transform.translation.z += amount.y;
    }

    void Context::RegisterLuaNatives() {
        auto ves_namespace = lua["ves"].get_or_create<sol::table>();

        ves_namespace.new_usertype<Vector2>("vec2", "x", &Vector2::x, "y", &Vector2::y);

        ves_namespace.set_function("planar_move", &Context::PlanarMove, this);
    }
}
