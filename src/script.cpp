#include "include/ves.hpp"

namespace VES {
    void Context::RegisterLuaNatives() {
        auto ves_namespace = lua["ves"].get_or_create<sol::table>();

        ves_namespace.set_function("planar_move", [this](entt::entity entity, sol::table amount) {
            Component::Transform& transform = world.get<Component::Transform>(entity);
            transform.translation.x += float(amount["x"]);
            transform.translation.z += float(amount["y"]);

            if (Component::Blockable* blockable = world.try_get<Component::Blockable>(entity)) {
                world.view<Component::Transform, Component::UnboundedVerticalBlock, Component::Renderable>().each([this /*TODO: Emily remove*/, blockable, &transform, &amount](entt::entity entity, Component::Transform& other, Component::UnboundedVerticalBlock& block, Component::Renderable& renderable) {
                    if (transform.translation.x + (blockable->bounds.max.x * transform.scale.x) > other.translation.x + (block.bounds.min.x * other.scale.x) && transform.translation.x + (blockable->bounds.min.x * transform.scale.x) < other.translation.x + (block.bounds.max.x * other.scale.x)) {
                        if (transform.translation.z + (blockable->bounds.max.z * transform.scale.z) > other.translation.z + (block.bounds.min.z * other.scale.z) && transform.translation.z + (blockable->bounds.min.z * transform.scale.z) < other.translation.z + (block.bounds.max.z * other.scale.z)) {
                            transform.translation.x -= float(amount["x"]);
                            transform.translation.z -= float(amount["y"]);
                        }
                    }
                });
            }
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
