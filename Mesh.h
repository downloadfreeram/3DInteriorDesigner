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

class VAOManager; 

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
    std::string type;
    std::string path;
};

class Mesh {
public:
    // mesh Data
    vector<Vertex>       vertices;
    vector<unsigned int> indices;
    vector<Texture>      textures;

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

    void setVAO(unsigned int vao) const;
    void Draw(const Shader& shader) const;
    // initializes all the buffer objects/arrays
    void setupMesh();
private:
    mutable unsigned int VAO;
    // render data 
    unsigned int VBO, EBO;
};

class VAOManager {
public:
    // singleton
    static VAOManager& getInstance() {
        static VAOManager instance;
        return instance;
    }
    unsigned int getVAO(const Mesh& mesh) {
        std::string layoutKey = getLayoutKey(mesh);
        auto it = vaoMap.find(layoutKey);

        if (it != vaoMap.end()) {
            //VAO already exists for this layout
            return it->second;
        } 
        else {
            // create a new VAO for this layout
            unsigned int vao = createVAO(mesh);
            vaoMap[layoutKey] = vao;
            // Check if createVAO was successful
            if (vao != 0) {
                vaoMap[layoutKey] = vao;
                return vao;
            }
            else {
                std::cout << "ERROR!" << std::endl;
                return 0;
            }
            return vao;
        }
    }
private:
    VAOManager() {};
    std::map<std::string, unsigned int> vaoMap;

    //generaets a unique key based on the mesh vertes layout
    std::string getLayoutKey(const Mesh& mesh) {
        // for example generate a key based on vertes attributes
        std::string key;
        // append other attributes as necessary
        key += "P:" + std::to_string(sizeof(mesh.vertices[0].Position));
        key += "N:" + std::to_string(sizeof(mesh.vertices[0].Normal));
        key += "T:" + std::to_string(sizeof(mesh.vertices[0].TexCoords));
        return key;
    }
    unsigned int createVAO(const Mesh& mesh) {
        unsigned int vao;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        //bind and set vertex buffer and attribute pointer
        unsigned int vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vertex), &mesh.vertices[0], GL_STATIC_DRAW);

        //position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(0);

        //normal attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,Normal));
        glEnableVertexAttribArray(1);

        //texture attribute
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
        glEnableVertexAttribArray(2);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        mesh.setVAO(vao);
        return vao;
    }
};
#endif

