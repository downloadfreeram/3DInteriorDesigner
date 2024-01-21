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

    MeshSnapshot() = default;

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
    std::string objectName;
    std::string textureName;
    std::vector<MeshSnapshot> meshes;
    std::vector<Texture> textures;
    ShaderSnapshot shader;

    // default constructor
    ModelSnapshot() = default;

    ModelSnapshot(const Model& model)
        : position(model.getPosition()),
        rotation(model.getRotation()),
        scale(model.getScale()) {
        for (const auto& mesh : model.meshes) {  
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

        // Serialize object name
        size_t objectNameLength = objectName.length();
        os.write(reinterpret_cast<const char*>(&objectNameLength), sizeof(objectNameLength));
        os.write(objectName.c_str(), objectNameLength);

        // Serialize texture name
        size_t textureNameLength = textureName.length();
        os.write(reinterpret_cast<const char*>(&textureNameLength), sizeof(textureNameLength));
        os.write(textureName.c_str(), textureNameLength);

        // serialize meshes
        size_t numMeshes = meshes.size();
        os.write(reinterpret_cast<const char*>(&numMeshes), sizeof(numMeshes));
        for (const auto& mesh : meshes) {
            mesh.serialize(os);
        }

        // serialize textures
        size_t sizeTextures = textures.size();
        os.write(reinterpret_cast<const char*>(&sizeTextures), sizeof(sizeTextures));
        for (const auto& texture : textures) {
            size_t pathLength = texture.path.length();
            os.write(reinterpret_cast<const char*>(&pathLength), sizeof(pathLength));
            os.write(texture.path.c_str(), pathLength);
        }
    }

    // deserialize the snapshot from an input stream
    void deserialize(std::istream& is) {
        is.read(reinterpret_cast<char*>(&position), sizeof(position));
        is.read(reinterpret_cast<char*>(&rotation), sizeof(rotation));
        is.read(reinterpret_cast<char*>(&scale), sizeof(scale));

        // Deserialize object name
        size_t objectNameLength;
        is.read(reinterpret_cast<char*>(&objectNameLength), sizeof(objectNameLength));
        char* objectNameBuffer = new char[objectNameLength + 1];
        is.read(objectNameBuffer, objectNameLength);
        objectNameBuffer[objectNameLength] = '\0';
        objectName = std::string(objectNameBuffer);
        delete[] objectNameBuffer;

        // Deserialize texture name
        size_t textureNameLength;
        is.read(reinterpret_cast<char*>(&textureNameLength), sizeof(textureNameLength));
        char* textureNameBuffer = new char[textureNameLength + 1];
        is.read(textureNameBuffer, textureNameLength);
        textureNameBuffer[textureNameLength] = '\0';
        textureName = std::string(textureNameBuffer);
        delete[] textureNameBuffer;

        // Deserialize meshes
        size_t numMeshes;
        is.read(reinterpret_cast<char*>(&numMeshes), sizeof(numMeshes));
        meshes.resize(numMeshes);
        for (auto& mesh : meshes) {
            mesh.deserialize(is);
        }

        size_t sizeTextures;
        is.read(reinterpret_cast<char*>(&sizeTextures), sizeof(sizeTextures));
        textures.resize(sizeTextures);
        for (auto& texture : textures) {
            size_t pathLength;
            is.read(reinterpret_cast<char*>(&pathLength), sizeof(pathLength));
            texture.path.resize(pathLength);
            is.read(&texture.path[0], pathLength);
            texture.id = TextureFromFile(texture.path.c_str(), "resources/objects");
        }
        
    }
};

#endif // MODEL_SNAPSHOT_H
