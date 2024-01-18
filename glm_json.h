#pragma once

#include <glm/glm.hpp>
#include <nlohmann/json.hpp>

namespace glm {

    nlohmann::json mat4_to_json(const glm::mat4& mat);
    void mat4_from_json(const nlohmann::json& j, glm::mat4& mat);

}
namespace glm_json {
    nlohmann::json mat4_to_json_array(const glm::mat4& matrix);
    void mat4_from_json_array(const nlohmann::json& j, glm::mat4& matrix);
}

