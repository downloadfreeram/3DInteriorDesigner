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
        os.write(reinterpret_cast<const char*>(&vertices), sizeof(vertices));
        os.write(reinterpret_cast<const char*>(&indices), sizeof(indices));
        os.write(reinterpret_cast<const char*>(&textures), sizeof(textures));
    }
    // deserialize the snapshot from an input stream
    void deserialize(std::istream& is) {
        is.read(reinterpret_cast<char*>(&vertices), sizeof(vertices));
        is.read(reinterpret_cast<char*>(&indices), sizeof(indices));
        is.read(reinterpret_cast<char*>(&textures), sizeof(textures));
        // Deserialize other properties here
    }
};

struct ShaderSnapshot {
    std::string vertexShaderPath;
    std::string fragmentShaderPath;

    ShaderSnapshot(const Shader& shader)
        :vertexShaderPath(shader.vertexShaderPath), fragmentShaderPath(shader.fragmentShaderPath) {}

    void applyToShader(Shader& shader) const {
        shader.vertexShaderPath = vertexShaderPath;
        shader.fragmentShaderPath = fragmentShaderPath;
        shader.recompileAndRelink();
    }
    void serialize(std::ostream& os) const {
        os.write(reinterpret_cast<const char*>(&vertices), sizeof(vertices));
        os.write(reinterpret_cast<const char*>(&indices), sizeof(indices));
    }
    // deserialize the snapshot from an input stream
    void deserialize(std::istream& is) {
        is.read(reinterpret_cast<char*>(&vertices), sizeof(vertices));
        is.read(reinterpret_cast<char*>(&indices), sizeof(indices));
        // Deserialize other properties here
    }
};
struct ModelSnapshot {
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    std::vector<MeshSnapshot> meshes;
    std::vector<Texture> textures;
    ShaderSnapshot shader;
    // Include any additional properties that are important for your model

    // default constructor
    ModelSnapshot() = default;

    // constructor to create a snapshot from a Model
    ModelSnapshot(const Model& model)
        : position(model.getPosition()),
        rotation(model.getRotation()),
        scale(model.getScale()) {
        for (const& auto mesh : model.meshes) {
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
