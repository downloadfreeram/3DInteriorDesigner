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
        this->vertexShaderPath = vertexPath;
        this->fragmentShaderPath = fragmentPath;
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

        // compile vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");

        // compile fragment Shader
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
    void setVec2(const std::string& name, const glm::vec2& value)
    {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
        vec2Value = value;
    }
    void setVec2(const std::string& name, float x, float y) 
    {
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
        vec2Value = glm::vec2(x, y);
    }
    void setVec3(const std::string& name, const glm::vec3& value)
    {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
        vec3Value = value;
    }
    void setVec3(const std::string& name, float x, float y, float z) 
    {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
        vec3Value = glm::vec3(x, y, z);
    }
    void setVec4(const std::string& name, const glm::vec4& value)
    {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
        vec4Value = value;
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
    void recompileAndRelink() {
        // 1. Retrieve the vertex/fragment source code from file paths
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;

        // Ensuring ifstream objects can throw exceptions
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        try {
            vShaderFile.open(vertexShaderPath);
            fShaderFile.open(fragmentShaderPath);
            std::stringstream vShaderStream, fShaderStream;

            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();

            vShaderFile.close();
            fShaderFile.close();

            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        catch (std::ifstream::failure& e) {
            std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
            return;
        }

        // 2. Compile shaders
        unsigned int vertex, fragment;
        // Vertex Shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        const char* vShaderCode = vertexCode.c_str();
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");

        // Fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        const char* fShaderCode = fragmentCode.c_str();
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");

        // Shader Program
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");

        // Delete shaders as they're linked into our program now
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }

private:
    void readShaderCode(const std::string& path, std::string& shaderCode) {
        std::ifstream shaderFile;
        shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try {
            shaderFile.open(path);
            std::stringstream shaderStream;
            shaderStream << shaderFile.rdbuf();
            shaderFile.close();
            shaderCode = shaderStream.str();
        }
        catch (std::ifstream::failure& e) {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
            std::cout << path << std::endl;
            std::cout << shaderCode << std::endl;
        }
    }

    
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

