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


std::vector<Model> models;  //Vector of objects
std::vector<std::string> modelNames;    //Vector of objects names
    
int selectedId = -1; // Index of the selected model

//transform variables
glm::vec3 posXYZ = glm::vec3(0.0f, 0.0f, 0.0f);
float rot = 0.0f;

// Flag to indicate if the model should be generated/rendered
bool generateModel = false; 

std::string object;
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
std::string GenerateUniqueName(const std::string& defaultName) {
    int cnt = 0;
    std::string newName = defaultName;
    while (std::find(modelNames.begin(), modelNames.end(), newName) != modelNames.end()) {
        newName = defaultName + std::to_string(cnt);
        cnt++;
    }
    return newName;
}
void GenerateObject(std::string name, const char* texName, Shader& ourShader, int id, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale,std::string menuName) {
    // Load the model
    Model ourModel(string("resources/objects/") + name, id, position, rotation, scale);
    models.push_back(ourModel);

    std::string uniqueName = GenerateUniqueName(menuName); // Replace "ModelBaseName" with actual base name
    modelNames.push_back(uniqueName);
    

    // Load texture
    TextureFromFile(texName, "resources/objects");

    // Draw the model
    ourModel.Draw(ourShader);
}

void DeleteObject(std::string name,int id) {
    models.erase(models.begin()+id);
    modelNames.erase(modelNames.begin() + id);
}

void RenderModelWindow(GLFWwindow* window, Shader& ourShader) {
    ImGui::Begin("Viewport", &showModelWindow);
    if (ImGui::BeginCombo("Select Model", selectedId >= 0 ? modelNames[selectedId].c_str() : "None")) {
        for (int i = 0; i < modelNames.size(); i++) {
            bool isSelected = (selectedId == i);
            if (ImGui::Selectable(modelNames[i].c_str(), isSelected)) {
                selectedId = i;
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    if (selectedId >= 0 && selectedId < models.size()) {
        ImGui::SliderFloat3("Position", glm::value_ptr(models[selectedId].position), -10.0f, 10.0f);
        ImGui::SliderFloat3("Rotation", glm::value_ptr(models[selectedId].rotation), 0.0f, 360.0f);
        if (ImGui::Button("Delete")) {
            DeleteObject(modelNames[selectedId], selectedId);
            // After deleting an object update the id
            if (models.empty()) {
                selectedId = -1; 
            }
            else {
                selectedId = std::min(selectedId, static_cast<int>(models.size()) - 1); 
            }
        }
    }

    //enable shader 
    ourShader.use();
    ourShader.setMat4("camMatrix", camera.cameraMatrix);

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) 
    {
        ImGui::OpenPopup("Generate");
    } 

    if (ImGui::BeginPopup("Generate")) 
    {
        if (ImGui::Button("Generate Cube")) {
            GenerateObject("untitled.obj", "texture_diffuse1.jpg", ourShader, models.size(), posXYZ, glm::vec3(0.0f, glm::radians(rot), 0.0f), glm::vec3(1),"Cube");
        }


        if (ImGui::Button("Generate Chair")) {
            GenerateObject("test.obj", "texture_diffuse1.jpg", ourShader, models.size(), posXYZ, glm::vec3(0.0f, glm::radians(rot), 0.0f), glm::vec3(1),"Chair");
        }

        ImGui::EndPopup();
    }

    //loop to iterate each model in vector
    for (Model& model : models) {
        // Set model-specific transformations (position, rotation, scale)
        glm::mat4 modelMatrix = model.GetTransformMatrix();
        // You can use a model-specific shader or a general one, depending on your design
        ourShader.setMat4("model", modelMatrix);
        model.Draw(ourShader);
    }

    // ImGui input handling
    bool ImGuiHandlingInput = ImGui::GetIO().WantCaptureMouse;

    // Process camera inputs only if ImGui is not handling input
    if (!ImGuiHandlingInput) {
        camera.updateMatrix(camera.zoom, 0.1f, 100.0f);
        camera.Inputs(window);
    }

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

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Interior Designer", NULL, NULL);
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
            RenderModelWindow(window, ourShader);
        }


        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // delete all resources
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}

