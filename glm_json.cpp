#include "glm_json.h"

namespace glm {

    nlohmann::json mat4_to_json(const glm::mat4& mat) {
        nlohmann::json j;
        for (int i = 0; i < 4; ++i) {
            for (int k = 0; k < 4; ++k) {
                j.push_back(mat[i][k]);
            }
        }
        return j;
    }

    void mat4_from_json(const nlohmann::json& j, glm::mat4& mat) {
        for (int i = 0; i < 4; ++i) {
            for (int k = 0; k < 4; ++k) {
                mat[i][k] = j[i * 4 + k].get<float>();
            }
        }
    }

} 
namespace glm_json {

    // Convert glm::mat4 to a JSON array
    nlohmann::json mat4_to_json_array(const glm::mat4& matrix) {
        nlohmann::json j;
        for (int i = 0; i < 4; ++i) {
            for (int k = 0; k < 4; ++k) {
                j.push_back(matrix[i][k]);
            }
        }
        return j;
    }

    // Convert a JSON array to glm::mat4
    void mat4_from_json_array(const nlohmann::json& j, glm::mat4& matrix) {
        if (j.size() == 16) {
            for (int i = 0; i < 4; ++i) {
                for (int k = 0; k < 4; ++k) {
                    matrix[i][k] = j[i * 4 + k];
                }
            }
        }
        else {
            // Handle error, throw an exception, or provide a default value.
            // For simplicity, let's set the matrix to an identity matrix.
            matrix = glm::mat4(1.0f);
        }
    }

}  
