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
#include "Snapshot.h"

#include <iostream>
#include <chrono>
#define NOMINMAX
#ifdef _WIN32
#include <windows.h>
#endif
#include <commdlg.h>  



// settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 1200;

// camera
Camera camera(SCR_WIDTH, SCR_HEIGHT, 45.0f, glm::vec3(0.0f, 0.0f, 2.0f));

// room
Model room;

Shader ourShader;

//menu logic
bool showMainMenu = true;
bool showSecondaryWindow = false;
bool showModelWindow = false;
bool showChooseWindow = false;
bool ImGuiHandlingInput = false;


std::vector<Model> models;  //vector of objects
std::vector<std::string> modelNames;    //vector of objects names
    
int selectedId = -1; // id of the selected model

//transform variables
glm::vec3 posXYZ = glm::vec3(0.0f, 0.0f, 0.0f);
float rot = 0.0f;

std::string OpenFileDialog() {
    OPENFILENAME ofn;       // common dialog box structure
    char szFile[260];       // buffer for file name
    HWND hwnd = NULL;       // owner window
    HANDLE hf;              // file handle

    // Initialize OPENFILENAME
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = szFile;
    // Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
    // use the contents of szFile to initialize itself.
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "BIN Files (*.bin)\0*.bin\0All Files (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    // Display the Open dialog box. 
    if (GetOpenFileName(&ofn) == TRUE) {
        hf = CreateFile(ofn.lpstrFile,
            GENERIC_READ,
            0,
            (LPSECURITY_ATTRIBUTES)NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            (HANDLE)NULL);
        return ofn.lpstrFile;
    }
    return "";
}
std::string SaveFileDialog() {
    OPENFILENAME ofn;
    char szFile[260];
    HWND hwnd = NULL;
    HANDLE hf;

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "BIN Files (*.bin)\0*.bin\0All Files (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

    if (GetSaveFileName(&ofn) == TRUE) {
        std::string filepath = ofn.lpstrFile;
        if (filepath.find_last_of(".") == std::string::npos) {
            filepath += ".bin";
        }
        hf = CreateFile(ofn.lpstrFile,
            GENERIC_WRITE,
            0,
            (LPSECURITY_ATTRIBUTES)NULL,
            CREATE_NEW,
            FILE_ATTRIBUTE_NORMAL,
            (HANDLE)NULL);
        return filepath;
    }
    return "";
}

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
void GenerateObject(std::string name, const std::string& texName, Shader& ourShader, int id, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale,std::string menuName) {
    // load the model
    Model ourModel(string("resources/objects/") + name, id, position, rotation, scale);
    ourModel.objectName = menuName;
    ourModel.textureName = texName;
    models.push_back(ourModel);

    std::string uniqueName = GenerateUniqueName(menuName); 
    modelNames.push_back(uniqueName);
    

    // load texture
    GLuint textureID = TextureFromFile(texName.c_str(), "resources/objects");

    // set the texture for the generated model
    ourModel.textures_loaded.clear(); // clear existing textures (if any)
    Texture texture;
    texture.id = textureID;
    texture.type = "texture_diffuse"; 
    texture.path = texName;
    ourModel.textures_loaded.push_back(texture);

    // draw the model
    ourModel.Draw(ourShader);
}

// function to delete a specific object
void DeleteObject(std::string name,int id) {
    models.erase(models.begin()+id);
    modelNames.erase(modelNames.begin() + id);
}
// vector to determine which room to load
std::vector<std::string> roomModelNames = { "room.obj", "room1.obj" };
std::string selectedRoomModel;

void saveGameState(const std::string& filepath, const std::vector<Model>& models);
void loadGameState(const std::string& filepath, std::vector<Model>& models, Shader& ourShader, string& selectedRoomModel);

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
        std::string filepath = OpenFileDialog();
        if (!filepath.empty()) {
            loadGameState(filepath, models, ourShader, selectedRoomModel);
            DisplayModelWindow(); 
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
// load and store textures for icons
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

void RenderModelWindow(GLFWwindow* window, Shader& ourShader) {
    //enable shader 
    ourShader.use();
    ourShader.setMat4("camMatrix", camera.cameraMatrix);

    ourShader.setMat4("model", room.GetTransformMatrix());
    room.Draw(ourShader);

    ImGuiHandlingInput = ImGui::GetIO().WantCaptureMouse;

    // process camera inputs only if ImGui is not handling input
    if (!ImGuiHandlingInput) {
        camera.updateMatrix(camera.zoom, 0.1f, 100.0f);
        camera.Inputs(window);
    }



    ImGui::Begin("Viewport", &showModelWindow);

    for (auto& name : modelNames) {
        if (name.empty()) {
            std::cerr << "ERROR: empty model name!" << std::endl;
            name = "none";
        }
    }
    // dropdown menu for every object
    const char* combo_preview = selectedId >= 0 ? modelNames[selectedId].c_str() : "Select a model";
    if (ImGui::BeginCombo("Model", combo_preview)) {
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
        ImGui::SliderFloat("Y Position", &models[selectedId].position.y, -20.0f, 20.0f);
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
        if (ImGui::Button("Standard Chair")) {
            GenerateObject("chair_normal.obj", "texture_diffuse1.jpg", ourShader, models.size(), posXYZ, glm::vec3(0.0f, glm::radians(rot), 0.0f), glm::vec3(1),"Standard Chair");
        }


        if (ImGui::Button("Dresser")) {
            GenerateObject("dresser.obj", "texture_diffuse3.jpg", ourShader, models.size(), posXYZ, glm::vec3(0.0f, glm::radians(rot), 0.0f), glm::vec3(1),"Dresser");
        }

        ImGui::EndPopup();
    }

    // Batch rendering of all models
    for (const Model& model : models) {
        // Set transformations for specific object
        glm::mat4 modelMatrix = model.GetTransformMatrix();
        ourShader.setMat4("model", modelMatrix);

        // Draw the model
        model.Draw(ourShader);
    }

    bool ImGuiHandlingInput = ImGui::GetIO().WantCaptureMouse;

    // process camera inputs only if ImGui is not handling input
    if (!ImGuiHandlingInput) {
        camera.updateMatrix(camera.zoom, 0.1f, 100.0f);
        camera.Inputs(window);
    }
    // Handle 'Q' key for Save
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        std::string filepath = SaveFileDialog();
        if (!filepath.empty()) {
            saveGameState(filepath, models);
            std::cout << filepath << std::endl;
            std::cout << "Scene has been successfully saved to " << filepath << std::endl;
        }
    }

    ImGui::End();
}
void saveGameState(const std::string& filepath, const std::vector<Model>& models) {
    std::ofstream outFile(filepath, std::ios::binary);
    if (!outFile) {
        throw std::runtime_error("Failed to open file for saving");
    }

    for (const auto& model : models) {
        std::cout << model.objectName << std::endl;
        ModelSnapshot snapshot(model);
        snapshot.serialize(outFile);
    }

    // Close the file before changing permissions
    outFile.close();

#ifdef _WIN32
    // Change file attributes to make it writable on Windows
    if (!SetFileAttributes(filepath.c_str(), FILE_ATTRIBUTE_NORMAL)) {
        throw std::runtime_error("Failed to change file attributes");
    }
#endif

    std::cout << filepath << std::endl;
    std::cout << "Scene has been successfully saved to " << filepath << std::endl;
}


void loadGameState(const std::string& filepath, std::vector<Model>& models, Shader& shader, std::string& selectedRoomModel) {
    std::cout << "Attempting to load from file: " << filepath << std::endl;
    std::ifstream inFile(filepath, std::ios::binary);


    if (!inFile) {
        perror("Error opening file");
        throw std::runtime_error("Failed to open file for loading");
    }

    models.clear(); // Clear existing models

    while (inFile.peek() != EOF) {
        ModelSnapshot snapshot;
        snapshot.deserialize(inFile);

        Model model;
        model.setPosition(snapshot.position);
        model.setRotation(snapshot.rotation);
        model.setScale(snapshot.scale);
        model.objectName = snapshot.objectName;
        model.textureName = snapshot.textureName;

        if (!model.objectName.empty()) {
            modelNames.push_back(model.objectName);
        }
        else {
            std::cerr << "WARNING: Loaded model with an empty name" << std::endl;
        }

        // Load model meshes
        for (auto& meshSnapshot : snapshot.meshes) {
            Mesh mesh;
            meshSnapshot.applyToMesh(mesh);
            model.meshes.push_back(mesh);
        }

        // Load model textures
        for (const auto& textureSnapshot : snapshot.textures) {
            Texture texture;
            texture.id = TextureFromFile(textureSnapshot.path.c_str(), "resources/objects");
            texture.type = textureSnapshot.type;
            texture.path = textureSnapshot.path;
            model.textures_loaded.push_back(texture);
        }

        // Apply shader snapshot
        ShaderSnapshot shaderSnapshot = snapshot.shader;
        shader.vertexShaderPath = shaderSnapshot.vertexShaderPath;
        shader.fragmentShaderPath = shaderSnapshot.fragmentShaderPath;
        shader.recompileAndRelink();

        models.push_back(model);

    }
    selectedRoomModel = "room1.obj";
    initializeScene(shader, "texture_diffuse2.jpg", selectedRoomModel);
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
            initializeScene(ourShader,"texture_diffuse2.jpg",selectedRoomModel);
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

