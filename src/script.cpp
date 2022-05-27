#include "include/ves.hpp"

namespace VES {
    void Context::RegisterLuaNatives() {
        auto ves_namespace = lua["ves"].get_or_create<sol::table>();

        ves_namespace.set_function("planar_move", [this](entt::entity entity, sol::table amount) {
            Component::Transform& transform = world.get<Component::Transform>(entity);
            transform.translation.x += float(amount["x"]);
            transform.translation.z += float(amount["y"]);

            world.view<Component::Transform, Component::UnboundedVerticalBlock>().each([&transform, &amount](entt::entity entity, Component::Transform& other, Component::UnboundedVerticalBlock block) {
                fmt::print("A: ({},{}) {}x{} B: ({},{}) {}x{}\n", transform.translation.x, transform.translation.z, transform.scale.x, transform.scale.z, other.translation.x, other.translation.z, other.scale.x, other.scale.z);
                if (transform.translation.x < other.translation.x + other.scale.x && transform.translation.x + transform.scale.x > other.translation.x) {
                    if (transform.translation.z < other.translation.z + other.scale.z && transform.translation.z + transform.scale.z > other.translation.z) {
                        transform.translation.x -= float(amount["x"]);
                        transform.translation.z -= float(amount["y"]);
                    }
                }
            });
        });

        ves_namespace.set_function("get_entity", [this](std::string name) {
            return scene[name];
        });

        ves_namespace.set_function("register", [this](std::string signal, sol::protected_function f, std::string name) {
            entt::entity entity = scene[name];
            Component::LuaBehavior& behavior = world.get<Component::LuaBehavior>(entity);
            behavior.callbacks[signal] = f;
        });
    }
}
