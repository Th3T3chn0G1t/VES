#include "include/ves.hpp"

namespace VES {
    Context* MapSolver::ctx{nullptr};

    // Return the estimated cost to goal from this node (pass reference to goal node)
    float MapSolver::GoalDistanceEstimate(MapSolver& goal) {
        return std::sqrt(std::pow(pos.x - goal.pos.x, 2) + std::pow(pos.y - goal.pos.y, 2));
    }

    // Return true if this node is the goal.
    bool MapSolver::IsGoal(MapSolver& goal) {
        return IsSameState(goal);
    }

    // For each successor to this state call the AStarSearch's AddSuccessor call to
    // add each one to the current search - return false if you are out of memory and the search
    // will fail
    bool MapSolver::GetSuccessors(MapSolver::Search* astarsearch, MapSolver* parent) {
        const glm::ivec2& parent_pos = parent ? parent->pos : glm::ivec2{-1, -1};

        if (!MapSolver::ctx->map->grid[(pos.x - 1) + pos.y * MapSolver::ctx->map->width].occupier) {
            const glm::ivec2 npos{pos.x - 1, pos.y};
            if (npos != parent_pos) {
                MapSolver node(npos);
                astarsearch->AddSuccessor(node);
            }
        }

        if (!MapSolver::ctx->map->grid[pos.x + (pos.y - 1) * MapSolver::ctx->map->width].occupier) {
            const glm::ivec2 npos{pos.x, pos.y - 1};
            if (npos != parent_pos) {
                MapSolver node(npos);
                astarsearch->AddSuccessor(node);
            }
        }

        if (!MapSolver::ctx->map->grid[(pos.x + 1) + pos.y * MapSolver::ctx->map->width].occupier) {
            const glm::ivec2 npos{pos.x + 1, pos.y};
            if (npos != parent_pos) {
                MapSolver node(npos);
                astarsearch->AddSuccessor(node);
            }
        }

        if (!MapSolver::ctx->map->grid[pos.x + (pos.y + 1) * MapSolver::ctx->map->width].occupier) {
            const glm::ivec2 npos{pos.x, pos.y + 1};
            if (npos != parent_pos) {
                MapSolver node(npos);
                astarsearch->AddSuccessor(node);
            }
        }

        return true;
    }

    // Return the cost moving from this state to the state of successor
    float MapSolver::GetCost(MapSolver& successor) {
        glm::vec2 origin_pos = MapSolver::ctx->map->CellPosPlanarToWorld(pos);
        glm::vec2 moved_pos = MapSolver::ctx->map->CellPosPlanarToWorld(successor.pos);

        // TODO: This fetches the terrain transform 3 times. Maybe cache it in the map or something?
        const Component::Transform terrain_transform = MapSolver::ctx->world.get<Component::Transform>(MapSolver::ctx->map->terrain);
        float origin_height = (MapSolver::ctx->HeightAtPlanarWorldPos(origin_pos) - terrain_transform.translation.y) / terrain_transform.scale.y;
        float moved_height = (MapSolver::ctx->HeightAtPlanarWorldPos(moved_pos) - terrain_transform.translation.y) / terrain_transform.scale.y;

        // TODO: This should be unneccesary just trying to make sure I'm not stupid.
        assert(
            origin_height >= 0.0f && origin_height <= 1.0f &&
            moved_height >= 0.0f && moved_height <= 1.0f);

        return moved_height - origin_height;
    }

    // Return true if the provided state is the same as this state
    bool MapSolver::IsSameState(MapSolver& rhs) {
        return rhs.pos == pos;
    }
}
