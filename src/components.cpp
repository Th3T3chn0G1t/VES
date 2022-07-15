#include "include/ves.hpp"

namespace VES {
    Component::Transform::operator glm::mat4() {
        glm::mat4 mat(1.0f);

        glm::translate(mat, translation);
        glm::rotate(mat, rotation.x, {1.0f, 0.0f, 0.0f});
        glm::rotate(mat, rotation.y, {0.0f, 1.0f, 0.0f});
        glm::rotate(mat, rotation.z, {0.0f, 0.0f, 1.0f});
        glm::scale(mat, scale);

        return mat;
    }
}
