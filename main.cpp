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
#include <chrono>


// settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 1200;

// camera
Camera camera(SCR_WIDTH, SCR_HEIGHT, 45.0f, glm::vec3(0.0f, 0.0f, 2.0f));

// room
Model room;

// shader
Shader ourShader;

//menu logic
bool showMainMenu = true;
bool showSecondaryWindow = false;
bool showModelWindow = false;
bool showChooseWindow = false;


std::vector<Model> models;  //vector of objects
std::vector<std::string> modelNames;    //vector of objects names
    
int selectedId = -1; // id of the selected model

//transform variables
glm::vec3 posXYZ = glm::vec3(0.0f, 0.0f, 0.0f);
float rot = 0.0f;

std::string object;
// function to handle the names of generated objects in the dropdown menu
std::string GenerateUniqueName(const std::string& defaultName) {
    int cnt = 0;
    std::string newName = defaultName;
    while (std::find(modelNames.begin(), modelNames.end(), newName) != modelNames.end()) {
        newName = defaultName + std::to_string(cnt);
        cnt++;
    }
    return newName;
}

// function to generate and render an object
void GenerateObject(std::string name, const char* texName, Shader& ourShader, int id, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale,std::string menuName) {
    // load the model
    Model ourModel(string("resources/objects/") + name, id, position, rotation, scale);
    models.push_back(ourModel);

    std::string uniqueName = GenerateUniqueName(menuName); 
    modelNames.push_back(uniqueName);
    

    // load texture
    TextureFromFile(texName, "resources/objects");

    // draw the model
    ourModel.Draw(ourShader);
}

// function to delete a specific object
void DeleteObject(std::string name,int id) {
    models.erase(models.begin()+id);
    modelNames.erase(modelNames.begin() + id);
}

// forward declaration
void initializeScene(Shader& ourShader, const char* texName, const std::string roomObj);
std::vector<Model> loadScene(const std::string& filename);

void DisplaySecondaryWindow() {
    showMainMenu = false;
    showSecondaryWindow = true;
}

void DisplayModelWindow() {
    showMainMenu = false;
    showChooseWindow = false;
    showModelWindow = true;
}

void DisplayChooseWindow() {
    showMainMenu = false;
    showChooseWindow = true;
    showModelWindow = false;
}

void MainMenu() {
    ImGui::Begin("Main Menu", &showMainMenu);

    if (ImGui::Button("About")) {
        DisplaySecondaryWindow();
    }

    if (ImGui::Button("Start")) {
        DisplayChooseWindow();
    }
    if (ImGui::Button("Load Scene")) {
        //models = loadScene("scene.json"); // Loads the scene from a file
        std::vector<Model> loadedModels = loadScene("scene.json");
        initializeScene(ourShader, "texture_diffuse1.jpg","room1.obj");
        if (!loadedModels.empty()) {
            DisplayModelWindow();
        }
        else {
            std::cout << "Failed to load the scene! " << std::endl;
        }
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
GLuint LoadTexture(const char* filename) {
    GLuint textureID;
    glGenTextures(1, &textureID);

    int width, height, channels;
    unsigned char* image = stbi_load(filename, &width, &height, &channels, 0);
    if (image) {
        GLenum format = (channels == 3) ? GL_RGB : GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(image);
    }
    else {
        std::cerr << "Failed to load texture: " << filename << " | Reason: " << stbi_failure_reason() << std::endl;
    }

    return textureID;
}

//global variables of textures
GLuint texture1, texture2, texture3, texture4;

void LoadTextures() {
    texture1 = LoadTexture("resources/images/image1.png");
    texture2 = LoadTexture("resources/images/image2.png");
    texture3 = LoadTexture("resources/images/image3.png");
    texture4 = LoadTexture("resources/images/image4.png");
}

// vector to determine which room to load
std::vector<std::string> roomModelNames = { "room.obj", "room1.obj" };
std::string selectedRoomModel;

void ChooseWindow() {
    ImGui::Begin("Choose a room", &showChooseWindow);
    ImGui::Text("Choose the room preset");

    // Number of buttons
    int numButtons = roomModelNames.size();

    ImGui::Columns(numButtons, nullptr, false);
    ImTextureID imguiTextureIDs[] = {
        reinterpret_cast<ImTextureID>(static_cast<intptr_t>(texture1)),
        reinterpret_cast<ImTextureID>(static_cast<intptr_t>(texture2)),
        reinterpret_cast<ImTextureID>(static_cast<intptr_t>(texture3)),
        reinterpret_cast<ImTextureID>(static_cast<intptr_t>(texture4))
    };

    // check if there are enough textures for buttons
    assert(numButtons <= sizeof(imguiTextureIDs) / sizeof(imguiTextureIDs[0]));
    ImVec2 imageSize(100, 100);

    ImGui::SetColumnWidth(0, 200.0f);
    ImGui::Image(imguiTextureIDs[0], imageSize);
    if (ImGui::Button("Preset #1")) {
        std::cout << "klik" << std::endl;
        showChooseWindow = false;
        showModelWindow = true;
        selectedRoomModel = roomModelNames[0];
    }
    ImGui::SetColumnWidth(1, 200.0f);
    ImGui::Image(imguiTextureIDs[1], imageSize);
    if (ImGui::Button("Preset #2")) {
        std::cout << "klik" << std::endl;
        showChooseWindow = false;
        showModelWindow = true;
        selectedRoomModel = roomModelNames[1];
    }

    ImGui::Columns(1);
    ImGui::End();
}


void initializeScene(Shader& ourShader,const char* texName,const std::string roomObj) {
    room = Model("resources/objects/"+roomObj, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    TextureFromFile(texName, "resources/objects");
    ourShader.use();
}

nlohmann::json serializeModel(Model& model) {
    nlohmann::json j;
    j["position"] = { model.getPosition().x, model.getPosition().y, model.getPosition().z };
    j["rotation"] = { model.getRotation().x, model.getRotation().y, model.getRotation().z };
    j["scale"] = { model.getScale().x, model.getScale().y, model.getScale().z };

    Shader& modelShader = model.getShader();
    nlohmann::json serializedShader = modelShader.serialize();
    j["shader"] = serializedShader;
    return j;
}
Model deserializeModel(const nlohmann::json& j) {
    Model model;
    // Set properties from JSON
    model.setPosition(glm::vec3(j["position"][0], j["position"][1], j["position"][2]));
    model.setRotation(glm::vec3(j["rotation"][0], j["rotation"][1], j["rotation"][2]));
    model.setScale(glm::vec3(j["scale"][0], j["scale"][1], j["scale"][2]));
    return model;
}
void saveScene(std::vector<Model>& models, const std::string& filename) {
    nlohmann::json scene;
    for (auto& model : models) {
        nlohmann::json serializedModel = serializeModel(model);
        Shader& modelShader = model.getShader();
        nlohmann::json serializedShader = modelShader.serialize();
        serializedModel["shader"] = serializedShader;
        
        scene["models"].push_back(serializedModel);
    }
    std::ofstream file(filename);
    if (file.is_open()) {
        file << scene.dump(4); // serialize with indendation for readability
        file.close();
        std::cout << "Scene saved to " << filename << std::endl;
    }
    else {
        std::cerr << "Unable to open file for writing: " << filename << std::endl;
    }
}

std::vector<Model> loadScene(const std::string& filename) {
    std::ifstream file(filename);
    nlohmann::json scene;
    file >> scene;
    std::vector<Model> models;
    for (const auto& jModel : scene["models"]) {
        Model model;
        model.deserialize(jModel);
        models.push_back(model);
    }
    return models;
}

void RenderModelWindow(GLFWwindow* window, Shader& ourShader) {
    //enable shader 
    ourShader.use();
    ourShader.setMat4("camMatrix", camera.cameraMatrix);

    ourShader.setMat4("model", room.GetTransformMatrix());
    room.Draw(ourShader);

    ImGui::Begin("Viewport", &showModelWindow);

    // dropdown menu for every object
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

    // options for every object generated
    if (selectedId >= 0 && selectedId < models.size()) {
        ImGui::SliderFloat("X Position", &models[selectedId].position.x, -20.0f, 20.0f);
        ImGui::SliderFloat("Y Position", &models[selectedId].position.y, 0.0f, 20.0f);
        ImGui::SliderFloat("Z Position", &models[selectedId].position.z, -20.0f, 20.0f);


        ImGui::SliderFloat("Rotation", &models[selectedId].rotation.y, 0.0f, 360.0f);
        if (ImGui::Button("Delete")) {
            DeleteObject(modelNames[selectedId], selectedId);
            // after deleting an object update the id, and check if the vector is empty
            if (models.empty()) {
                selectedId = -1; 
            }
            else {
                selectedId = std::min(selectedId, static_cast<int>(models.size()) - 1); 
            }
        }
    }


    // after clicking the R button show the list of objects to generate
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
        // set transformations for specific object
        glm::mat4 modelMatrix = model.GetTransformMatrix();
        ourShader.setMat4("model", modelMatrix);
        model.Draw(ourShader);
    }

    // ImGui input handling
    bool ImGuiHandlingInput = ImGui::GetIO().WantCaptureMouse;

    // process camera inputs only if ImGui is not handling input
    if (!ImGuiHandlingInput) {
        camera.updateMatrix(camera.zoom, 0.1f, 100.0f);
        camera.Inputs(window);
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        saveScene(models, "scene.json"); // Saves the current scene to a file
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
    std::cout << ourShader.vertexShaderPath << "   "<<ourShader.fragmentShaderPath << std::endl;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");


    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();  // Process GLFW events

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

        if (showMainMenu) {
            MainMenu();
        }

        if (showSecondaryWindow) {
            SecondaryWindow();
        }

        if (showChooseWindow) {
            ChooseWindow();
        }

        if (showModelWindow) {
            initializeScene(ourShader,"texture_diffuse1.jpg",selectedRoomModel);
            RenderModelWindow(window, ourShader);
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // delete all resources
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}

