#include "include/ves.hpp"

namespace VES {
    void Context::RegisterLuaNatives() {
        auto ves_namespace = lua["ves"].get_or_create<sol::table>();

        ves_namespace.set_function("planar_move", [this](entt::entity entity, sol::table amount) {
            Component::Transform& transform = world.get<Component::Transform>(entity);
            transform.translation.x += float(amount["x"]);
            transform.translation.z += float(amount["y"]);

            if (Component::Blockable* blockable = world.try_get<Component::Blockable>(entity)) {
                world.view<Component::Transform, Component::UnboundedVerticalBlock, Component::Renderable>().each([entity, blockable, &transform, &amount](entt::entity inner, Component::Transform& other, Component::UnboundedVerticalBlock& block, Component::Renderable& renderable) {
                    if (entity == inner)
                        return;
                    if (transform.translation.x + (blockable->bounds.max.x * transform.scale.x) > other.translation.x + (block.bounds.min.x * other.scale.x) && transform.translation.x + (blockable->bounds.min.x * transform.scale.x) < other.translation.x + (block.bounds.max.x * other.scale.x)) {
                        if (transform.translation.z + (blockable->bounds.max.z * transform.scale.z) > other.translation.z + (block.bounds.min.z * other.scale.z) && transform.translation.z + (blockable->bounds.min.z * transform.scale.z) < other.translation.z + (block.bounds.max.z * other.scale.z)) {
                            transform.translation.x -= float(amount["x"]);
                            transform.translation.z -= float(amount["y"]);
                        }
                    }
                });
            }
        });

        // TODO: `planar_move_toward` - does interp.

        ves_namespace.set_function("pathfind_planar", [this](entt::entity entity, sol::table destination, sol::table speed) {
            // TODO: Move out search init into `Navigable` component or equiv. to avoid reinitializing
            //       search object for every step.
            MapSolver::Search astarsearch;

            Component::Transform& transform = world.get<Component::Transform>(entity);
            MapSolver begin{map->GetCellPosPlanar({transform.translation.x, transform.translation.z})};
            MapSolver end{map->GetCellPosPlanar({destination["x"], destination["y"]})};

            astarsearch.SetStartAndGoalStates(begin, end);

            bool done = astarsearch.SearchStep() == MapSolver::Search::SEARCH_STATE_SUCCEEDED;

            MapSolver* state = astarsearch.GetSolutionStart();
            while (true) {
                fmt::print("|> Pathfind state - x: {} y(z): {}\n", state->pos.x, state->pos.y);
                MapSolver* newstate = astarsearch.GetSolutionNext();
                if (newstate == NULL) {
                    break;
                }
                state = newstate;
            }

            const glm::vec2 newpos = map->CellPosPlanarToWorld(state->pos);

            // TODO: Pathfind interp w/ speed - this just snaps to next state.
            //       See `planar_move_toward` since atm. pathfinding has no temporal consistency.
            transform.translation = {newpos.x, transform.translation.y, newpos.y};
            fmt::print("Got pathfind cellular planar newpos - x: {} y(z): {}\n", state->pos.x, state->pos.y);

            // This is a really annoying cleanup block but oh well.
            astarsearch.CancelSearch();
            astarsearch.SetStartAndGoalStates(begin, end); // Reset states to not early-exit for cleanup
            astarsearch.SearchStep();
            astarsearch.FreeSolutionNodes();
            astarsearch.EnsureMemoryFreed();

            return done;
        });

        ves_namespace.set_function("get_entity", [this](std::string name) {
            auto it = scene.find(name);
            if (it == scene.end()) {
                fmt::print("No such entity `{}`\n", name);
                exit(1);
            }
            return it->second;
        });

        ves_namespace.set_function("get_entity_pos_planar", [this](entt::entity entity) {
            Component::Transform& transform = world.get<Component::Transform>(entity);
            return lua.create_table_with("x", transform.translation.x, "y", transform.translation.z);
        });

        ves_namespace.set_function("get_cursor_pos_planar", [this]() {
            return lua.create_table_with("x", camera.camera.target.x, "y", camera.camera.target.z);
        });

        ves_namespace.set_function("register", [this](std::string signal, sol::protected_function f, std::string name) {
            auto it = scene.find(name);
            if (it == scene.end()) {
                fmt::print("No such entity `{}`\n", name);
                exit(1);
            }
            Component::LuaBehavior& behavior = world.get<Component::LuaBehavior>(it->second);
            behavior.callbacks[signal] = f;
        });
    }
}
