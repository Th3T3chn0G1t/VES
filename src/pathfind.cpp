#include "include/ves.hpp"

namespace VES {
    // Return the estimated cost to goal from this node (pass reference to goal node)
    float MapSolver::GoalDistanceEstimate(MapSolver& nodeGoal) {
        return 0.0f;
    }

    // Return true if this node is the goal.
    bool MapSolver::IsGoal(MapSolver& nodeGoal) {
        return true;
    }

    // For each successor to this state call the AStarSearch's AddSuccessor call to
    // add each one to the current search - return false if you are out of memory and the search
    // will fail
    bool MapSolver::GetSuccessors(MapSolver::Search* astarsearch, MapSolver* parent) {
        return true;
    }

    // Return the cost moving from this state to the state of successor
    float MapSolver::GetCost(MapSolver& successor) {
        return 1.0f;
    }

    // Return true if the provided state is the same as this state
    bool MapSolver::IsSameState(MapSolver& rhs) {
        return rhs.pos == pos;
    }
}
