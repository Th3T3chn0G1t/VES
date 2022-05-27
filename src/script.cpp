#include "include/ves.hpp"

namespace VES {
    void Context::RegisterLuaNatives() {
        auto ves_namespace = lua["ves"].get_or_create<sol::table>();

        ves_namespace.set_function("planar_move", [this](entt::entity entity, sol::table amount) {
            Component::Transform& transform = world.get<Component::Transform>(entity);
            transform.translation.x += float(amount["x"]);
            transform.translation.z += float(amount["y"]);

            // world.view<Component::UnboundedVerticalBlock>().each([](entt::entity entity, Component::UnboundedVerticalBlock block) {
            //     if ()
            // });
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
