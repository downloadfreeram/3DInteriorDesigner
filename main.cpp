#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Model.h"
#include "Camera.h"
#include "Shader.h"

#include <iostream>


// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(SCR_WIDTH, SCR_HEIGHT, 45.0f, glm::vec3(0.0f, 0.0f, 2.0f));

//menu logic
bool showMainMenu = true;
bool showSecondaryWindow = false;
bool showModelWindow = false;

//transform variables
glm::vec3 posXYZ = glm::vec3(0.0f, 0.0f, 0.0f);
float rot = 0.0f;

void DisplaySecondaryWindow() {
    showMainMenu = false;
    showSecondaryWindow = true;
}

void DisplayModelWindow() {
    showMainMenu = false;
    showModelWindow = true;
}

void MainMenu() {
    ImGui::Begin("Main Menu", &showMainMenu);

    if (ImGui::Button("About")) {
        DisplaySecondaryWindow();
    }

    if (ImGui::Button("Start")) {
        DisplayModelWindow();
    }

    ImGui::End();
}

void SecondaryWindow() {
    ImGui::Begin("Secondary Window", &showSecondaryWindow);

    if (ImGui::Button("Back to Main Menu")) {
        showSecondaryWindow = false;
        showMainMenu = true;
    }

    ImGui::End();
}
void RenderModelWindow(GLFWwindow* window, Shader& ourShader, Model& ourModel) {
    ImGui::Begin("Viewport", &showModelWindow);
    ImGui::SliderFloat3("Position", glm::value_ptr(posXYZ), 0.0f, 10.0f);
    ImGui::SliderFloat("Rotation", &rot, 0.0f, 360.0f);

    // ImGui input handling
    bool ImGuiHandlingInput = ImGui::GetIO().WantCaptureMouse;

    // Process camera inputs only if ImGui is not handling input
    if (!ImGuiHandlingInput) {
        camera.updateMatrix(camera.zoom, 0.1f, 100.0f);
        camera.Inputs(window);
    }
    //enable shader 
    ourShader.use();
    ourShader.setMat4("camMatrix", camera.cameraMatrix);

    // render the loaded model and set its transforms
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, posXYZ);
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
    model = glm::rotate(model, glm::radians(rot), glm::vec3(0.0f, 1.0f, 0.0f));

    ourShader.setMat4("model", model);
    ourModel.Draw(ourShader);

    ImGui::End();
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // flip on y-axis
    stbi_set_flip_vertically_on_load(true);

    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    Shader ourShader("default.vert", "default.frag");

    // load models
    Model ourModel("resources/objects/untitled.obj");

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");


    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();


        if (showMainMenu) {
            MainMenu();
        }

        if (showSecondaryWindow) {
            SecondaryWindow();
        }

        if (showModelWindow) {
            RenderModelWindow(window, ourShader, ourModel);
        }


        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

