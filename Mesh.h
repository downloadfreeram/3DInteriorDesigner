#ifndef MESH_H
#define MESH_H

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"

#include <string>
#include <vector>
using namespace std;

#define MAX_BONE_INFLUENCE 4

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
    //bone indexes which will influence this vertex
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    //weights from each bone
    float m_Weights[MAX_BONE_INFLUENCE];
};

struct Texture {
    unsigned int id;
    string type;
    string path;
};

class Mesh {
public:
    // mesh Data
    vector<Vertex>       vertices;
    vector<unsigned int> indices;
    vector<Texture>      textures;
    unsigned int VAO;

    //default constructor
    Mesh() : vertices(), indices(), textures() {}

    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;

        // set the vertex buffers and its attribute pointers.
        setupMesh();
    }

    // render the mesh
    void Draw(Shader& shader)
    {
        // bind appropriate textures
        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr = 1;
        unsigned int heightNr = 1;
        for (unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
            // retrieve texture number
            string number;
            string name = textures[i].type;
            if (name == "texture_diffuse")
                number = std::to_string(diffuseNr++);
            else if (name == "texture_specular")
                number = std::to_string(specularNr++); // transfer unsigned int to string
            else if (name == "texture_normal")
                number = std::to_string(normalNr++); // transfer unsigned int to string
            else if (name == "texture_height")
                number = std::to_string(heightNr++); // transfer unsigned int to string

            // set the sampler to the correct texture unit
            glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
            // bind the texture
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }

        // draw mesh
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // set everything back to defaults once configured.
        glActiveTexture(GL_TEXTURE0);
    }

    nlohmann::json serialize() const {
        nlohmann::json j;
        // Serialize vertex data
        std::vector<nlohmann::json> verticesJson;
        for (const Vertex& v : this->vertices) {
            verticesJson.push_back({ {"position", {v.Position.x, v.Position.y, v.Position.z}},
                                    {"normal", {v.Normal.x, v.Normal.y, v.Normal.z}},
                                    {"texCoords", {v.TexCoords.x, v.TexCoords.y}},
                                    { "tangent", {v.Tangent.x,v.Tangent.y,v.Tangent.z}},
                                    { "bitangent",{{v.Bitangent.x,v.Bitangent.y,v.Bitangent.z}}} });
        }
        j["vertices"] = verticesJson;

        //serialize indices
        j["indices"] = this->indices;

        //serialize textures
        std::vector<nlohmann::json> texturesJson;
        for (const Texture& t : this->textures) {
            texturesJson.push_back({ {"id",t.id},
                {"type",t.type},
                {"path",t.path} });
        }
        j["textures"] = texturesJson;
        return j;
    }


    void deserialize(const nlohmann::json& j) {
        // Deserialize vertex data
        std::vector<Vertex> vertices;
        for (const auto& vertexJson : j["vertices"]) {
            Vertex v;
            v.Position = glm::vec3(vertexJson["position"][0], vertexJson["position"][1], vertexJson["position"][2]);
            v.Normal = glm::vec3(vertexJson["normal"][0], vertexJson["normal"][1], vertexJson["normal"][2]);
            v.TexCoords = glm::vec2(vertexJson["texCoords"][0], vertexJson["texCoords"][1]);
            v.Tangent = glm::vec3(vertexJson["tangent"][0], vertexJson["tangent"][1], vertexJson["tangent"][2]);
            v.Bitangent = glm::vec3(vertexJson["bitangent"][0], vertexJson["bitangent"][1], vertexJson["bitangent"][2]);
            vertices.push_back(v);
        }
        this->vertices = vertices;

        //deserialize indices
        if (j.contains("indices") && j["indices"].is_array()) {
            this->indices = j["indices"].get<std::vector<unsigned int>>();
        }

        //deserialize textures
        if (j.contains("textures") && j["textures"].is_array()) {
            std::vector<Texture> textures;
            for (const auto& textureJson : j["textures"]) {
                Texture t;
                t.id = textureJson.value("id", 0);
                t.type = textureJson.value("type", "");
                t.path = textureJson.value("path", "");
                textures.push_back(t);
            }
            this->textures = textures;
        }
        setupMesh();
    }


private:
    // render data 
    unsigned int VBO, EBO;

    // initializes all the buffer objects/arrays
    void setupMesh()
    {
        // create buffers/arrays
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        // load data into vertex buffers
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // A great thing about structs is that their memory layout is sequential for all its items.
        // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
        // again translates to 3/2 floats which translates to a byte array.
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // set the vertex attribute pointers
        // vertex Positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // vertex normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        // vertex texture coords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
        // vertex tangent
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
        // vertex bitangent
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
        // ids
        glEnableVertexAttribArray(5);
        glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));
        // weights
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
        glBindVertexArray(0);
    }
};
#endif

