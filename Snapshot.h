#ifndef MODEL_SNAPSHOT_H
#define MODEL_SNAPSHOT_H

#include <iostream>
#include <glm/glm.hpp>
#include "Model.h"  
#include "Mesh.h"
#include "Shader.h"
struct MeshSnapshot {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    MeshSnapshot(const Mesh& mesh)
        : vertices(mesh.vertices), indices(mesh.indices), textures(mesh.textures) {}

    void applyToMesh(Mesh& mesh) {
        mesh.vertices = vertices;
        mesh.indices = indices;
        mesh.textures = textures;
        mesh.setupMesh();
    }
    void serialize(std::ostream& os) const {
        // Serialize vertices
        size_t verticesSize = vertices.size();
        os.write(reinterpret_cast<const char*>(&verticesSize), sizeof(verticesSize));
        for (const auto& vertex : vertices) {
            os.write(reinterpret_cast<const char*>(&vertex), sizeof(Vertex));
        }
        // Serialize indices 
        size_t indicesSize = indices.size();
        os.write(reinterpret_cast<const char*>(&indicesSize), sizeof(indicesSize));
        for (const auto& indices : indices) {
            os.write(reinterpret_cast<const char*>(&indices), sizeof(unsigned int));
        }
        // serialize textures
        size_t texturesSize = textures.size();
        os.write(reinterpret_cast<const char*>(&texturesSize), sizeof(texturesSize));
        for (const auto& texture : textures) {
            os.write(reinterpret_cast<const char*>(&texture), sizeof(Texture));
        }
    }

    void deserialize(std::istream& is) {
        // Deserialize vertices
        size_t verticesSize;
        is.read(reinterpret_cast<char*>(&verticesSize), sizeof(verticesSize));
        vertices.resize(verticesSize);
        for (auto& vertex : vertices) {
            is.read(reinterpret_cast<char*>(&vertex), sizeof(Vertex));
        }
        size_t indicesSize;
        is.read(reinterpret_cast<char*>(&indicesSize), sizeof(indicesSize));
        indices.resize(indicesSize);
        for (auto& indice : indices) {
            is.read(reinterpret_cast<char*>(&indice), sizeof(unsigned int));
        }

        size_t texturesSize;
        is.read(reinterpret_cast<char*>(&texturesSize), sizeof(texturesSize));
        textures.resize(texturesSize);
        for (auto& texture : textures) {
            is.read(reinterpret_cast<char*>(&texture), sizeof(Texture));
        }
    }

};

struct ShaderSnapshot {
    std::string vertexShaderPath;
    std::string fragmentShaderPath;
    
    // default constructor
    ShaderSnapshot() = default;

    ShaderSnapshot(const Shader& shader)
        :vertexShaderPath(shader.vertexShaderPath), fragmentShaderPath(shader.fragmentShaderPath) {}

    void applyToShader(Shader& shader) const {
        shader.vertexShaderPath = vertexShaderPath;
        shader.fragmentShaderPath = fragmentShaderPath;
        shader.recompileAndRelink();
    }
    void serialize(std::ostream& os) const {
        size_t pathLength;

        // Serialize vertex shader path
        pathLength = vertexShaderPath.length();
        os.write(reinterpret_cast<const char*>(&pathLength), sizeof(pathLength));
        os.write(vertexShaderPath.c_str(), pathLength);

        // Serialize fragment shader path
        pathLength = fragmentShaderPath.length();
        os.write(reinterpret_cast<const char*>(&pathLength), sizeof(pathLength));
        os.write(fragmentShaderPath.c_str(), pathLength);
    }

    void deserialize(std::istream& is) {
        size_t pathLength;
        char* buffer;

        // Deserialize vertex shader path
        is.read(reinterpret_cast<char*>(&pathLength), sizeof(pathLength));
        buffer = new char[pathLength + 1];
        is.read(buffer, pathLength);
        buffer[pathLength] = '\0';
        vertexShaderPath = std::string(buffer);
        delete[] buffer;

        // Deserialize fragment shader path
        is.read(reinterpret_cast<char*>(&pathLength), sizeof(pathLength));
        buffer = new char[pathLength + 1];
        is.read(buffer, pathLength);
        buffer[pathLength] = '\0';
        fragmentShaderPath = std::string(buffer);
        delete[] buffer;
    }

};
struct ModelSnapshot {
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    std::vector<MeshSnapshot> meshes;
    std::vector<Texture> textures;
    ShaderSnapshot shader;

    // default constructor
    ModelSnapshot() = default;

    ModelSnapshot(const Model& model)
        : position(model.getPosition()),
        rotation(model.getRotation()),
        scale(model.getScale()) {
        for (const auto& mesh : model.meshes) {  // Ensure model.meshes is accessible
            meshes.push_back(MeshSnapshot(mesh));
        }
        textures = model.textures_loaded;
        shader = ShaderSnapshot(model.getShader());
    }


    // apply this snapshot to a Model
    void applyToModel(Model& model) const {
        model.setPosition(position);
        model.setRotation(rotation);
        model.setScale(scale);
        // Set other properties to the model here
    }

    // serialize the snapshot to an output stream
    void serialize(std::ostream& os) const {
        os.write(reinterpret_cast<const char*>(&position), sizeof(position));
        os.write(reinterpret_cast<const char*>(&rotation), sizeof(rotation));
        os.write(reinterpret_cast<const char*>(&scale), sizeof(scale));
        // Serialize other properties here
    }

    // deserialize the snapshot from an input stream
    void deserialize(std::istream& is) {
        is.read(reinterpret_cast<char*>(&position), sizeof(position));
        is.read(reinterpret_cast<char*>(&rotation), sizeof(rotation));
        is.read(reinterpret_cast<char*>(&scale), sizeof(scale));
        // Deserialize other properties here
    }
};

#endif // MODEL_SNAPSHOT_H
