#ifndef CAMERA_H
#define CAMERA_H

#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>

#include"Shader.h"

class Camera
{
    public:
        glm::vec3 Position = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::mat4 cameraMatrix = glm::mat4(1.0f);
        bool firstClick = true;

        int width;
        int height;
        float zoom = 45.0f;
        float speed = 2.5f;
        float sensitivity = 20.0f;

        Camera(int width, int height,float zoom, glm::vec3 position);

        // Updates and exports the camera matrix to the Vertex Shader
        void updateMatrix(float FOVdeg, float nearPlane, float farPlane);
        // Handles camera inputs
        void Inputs(GLFWwindow* window);
        void ProcessMouseScroll(float yoffset);

};

#endif

