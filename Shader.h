#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <nlohmann/json.hpp>

class Shader
{
public:
    unsigned int ID;
    std::string vertexShaderPath;
    std::string fragmentShaderPath;
    //variables for serializing
    glm::vec4 vec4Value;
    glm::vec3 vec3Value;
    glm::vec2 vec2Value;
    float fVal;
    int iVal;
    bool bVal;
    glm::mat4 mat4Value;
    glm::mat3 mat3Value;
    glm::mat2 mat2Value;
    //default constructor
    Shader():ID(0) {}
    Shader(const char* vertexPath, const char* fragmentPath)
    {
        // 1. retrieve the vertex/fragment source code from filePath
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        // ensure ifstream objects can throw exceptions:
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            // open files
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            // read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            // close file handlers
            vShaderFile.close();
            fShaderFile.close();
            // convert stream into string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        catch (std::ifstream::failure& e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
        }
        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();
        // 2. compile shaders
        unsigned int vertex, fragment;
        // vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");
        // fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");
        // shader Program
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");
        // delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(vertex);
        glDeleteShader(fragment);

    }
    // activate the shader
    void use() const
    {
        glUseProgram(ID);
    }
    // utility uniform functions
    void setBool(const std::string& name, bool value) 
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
        bVal = value;
    }
    void setInt(const std::string& name, int value) 
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
        iVal = value;
    }
    void setFloat(const std::string& name, float value) 
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
        fVal = value;
    }
    void setVec2(const std::string& name, const glm::vec2& value) const
    {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec2(const std::string& name, float x, float y) 
    {
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
        vec2Value = glm::vec2(x, y);
    }
    void setVec3(const std::string& name, const glm::vec3& value) const
    {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec3(const std::string& name, float x, float y, float z) 
    {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
        vec3Value = glm::vec3(x, y, z);
    }
    void setVec4(const std::string& name, const glm::vec4& value) const
    {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec4(const std::string& name, float x, float y, float z, float w) 
    {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
        vec4Value = glm::vec4(x, y, z, w);
    }
    void setMat2(const std::string& name, const glm::mat2& mat) 
    {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
        mat2Value = mat;
    }
    void setMat3(const std::string& name, const glm::mat3& mat) 
    {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
        mat3Value = mat;
    }
    void setMat4(const std::string& name, const glm::mat4& mat) 
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
        mat4Value = mat;
    }
    // serialize data, turn objects into a string of data
    nlohmann::json serialize() const {
        nlohmann::json j;
        //Serialize vertex and fragment shader files
        j["vertexCode"] = vertexShaderPath;
        j["fragmentCode"] = fragmentShaderPath;
        // Serialize a bool uniform
        j["uniforms"]["boolUniform"] = {
            {"type", "bool"},
            {"value", bVal}
        };

        // Serialize an int uniform
        j["uniforms"]["intUniform"] = {
            {"type", "int"},
            {"value", iVal}
        };

        // Serialize a float uniform
        j["uniforms"]["floatUniform"] = {
            {"type", "float"}, 
            {"value", fVal}
        };

        // Serialize vec2, vec3, vec4 uniforms
        j["uniforms"]["vec2Uniform"] = {
            {"type","vec2"},
            {"value",{vec2Value.x,vec2Value.y}}
        };
        j["uniforms"]["vec3Uniform"] = {
            {"type","vec3"},
            {"value",{vec3Value.x,vec3Value.y,vec3Value.z}}
        };
        j["uniforms"]["vec4Uniform"] = {
            {"type", "vec4"},
            {"value", {vec4Value.x, vec4Value.y, vec4Value.z, vec4Value.w}}
        };

        // Serialize mat2, mat3, mat4 uniforms
        j["uniforms"]["mat4Uniform"] = {
            { "type", "mat4" },
            { "value", {mat4Value[0][0], mat4Value[0][1], mat4Value[1][0], mat4Value[1][1], mat4Value[1][2], mat4Value[2][1], mat4Value[2][0], mat4Value[0][2], mat4Value[2][2],mat4Value[3][0],mat4Value[3][1],mat4Value[3][2],mat4Value[0][3],mat4Value[1][3],mat4Value[2][3],mat4Value[3][3]} }
        };
        j["uniforms"]["mat3Uniform"] = {
            { "type", "mat3" }, 
            { "value", {mat3Value[0][0], mat3Value[0][1], mat3Value[1][0], mat3Value[1][1], mat3Value[1][2], mat3Value[2][1], mat3Value[2][0], mat3Value[0][2], mat3Value[2][2]} }
    };
        j["uniforms"]["mat2Uniform"] = { 
            {"type", "mat2" }, 
            {"value", {mat2Value[0][0], mat2Value[0][1], mat2Value[1][0], mat2Value[1][1]}}
        };
        return j;
    }

    // deserialize data, inverse of serialization
    void deserialize(const nlohmann::json& j) {
        // deserialize vertex and fragment shader files
        vertexShaderPath = j["vertexCode"];
        fragmentShaderPath = j["fragmentCode"];
        // Deserialize a bool uniform
        setBool(j["uniforms"]["boolUniform"]["name"], j["uniforms"]["boolUniform"]["value"]);

        // Deserialize an int uniform
        setInt(j["uniforms"]["intUniform"]["name"], j["uniforms"]["intUniform"]["value"]);

        // Deserialize a float uniform
        setFloat(j["uniforms"]["floatUniform"]["name"], j["uniforms"]["floatUniform"]["value"]);

        // Deserialize vec2, vec3, vec4 uniforms
        auto vec2Val = j["uniforms"]["vec2Uniform"]["value"];
        setVec2(j["uniforms"]["vec2Uniform"]["name"], vec2Val[0], vec2Val[1]);

        auto vec3Val = j["uniforms"]["vec3Uniform"]["value"];
        setVec3(j["uniforms"]["vec3Uniform"]["name"], vec3Val[0], vec3Val[1], vec3Val[2]);

        auto vec4Val = j["uniforms"]["vec4Uniform"]["value"];
        setVec4(j["uniforms"]["vec4Uniform"]["name"], vec4Val[0], vec4Val[1], vec4Val[2], vec4Val[3]);

        // Deserialize mat2, mat3, mat4 uniforms
        auto mat4Val = j["uniforms"]["mat4Uniform"]["value"];
        glm::mat4 mat4;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                mat4[i][j] = mat4Val[i * 4 + j];
            }
        }
        setMat4(j["uniforms"]["mat4Uniform"]["name"], mat4);

        auto mat3Val = j["uniforms"]["mat3Uniform"]["value"];
        glm::mat3 mat3;
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                mat3[i][j] = mat3Val[i * 3 + j];
            }
        }
        setMat3(j["uniforms"]["mat3Uniform"]["name"], mat3);

        auto mat2Val = j["uniforms"]["mat2Uniform"]["value"];
        glm::mat2 mat2;
        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 2; ++j) {
                mat2[i][j] = mat2Val[i * 2 + j];
            }
        }
        setMat2(j["uniforms"]["mat2Uniform"]["name"], mat2);
    }
private:
    // utility function for checking shader compilation/linking errors.
    void checkCompileErrors(GLuint shader, std::string type)
    {
        GLint success;
        GLchar infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }
};
#endif

