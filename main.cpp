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
const unsigned int CAM_WIDTH = 1920;
const unsigned int CAM_HEIGHT = 1080;

// camera
Camera camera(CAM_WIDTH, CAM_HEIGHT, 45.0f, glm::vec3(0.0f, 0.0f, 2.0f));

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

        // Check if the handle is valid
        if (hf == INVALID_HANDLE_VALUE) {
            std::cerr << "Error opening file: " << ofn.lpstrFile << std::endl;
            return "";
        }
        // Close the handle before returning
        CloseHandle(hf);
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
    Model ourModel("resources/objects/" + name, id, position, rotation, scale);
    ourModel.objectName = menuName;
    //ourModel.textureName = texName;

    std::string uniqueName = GenerateUniqueName(menuName); 
    modelNames.push_back(uniqueName);

    GLuint textureID = TextureFromFile(texName.c_str(), "resources/objects");

    // set the texture for the generated model
    ourModel.textures_loaded.clear(); // clear existing textures (if any)
    Texture texture;
    texture.id = textureID;
    texture.type = "texture_diffuse"; 
    texture.path = texName;
    ourModel.textures_loaded.push_back(texture);
    
    std::cout << "Texture ID for model " << menuName << ": " << textureID << std::endl;
    models.push_back(ourModel);
}

// function to delete a specific object
void DeleteObject(std::string name,int id) {
    models.erase(models.begin()+id);
    modelNames.erase(modelNames.begin() + id);
}
// vector to determine which room to load
std::vector<std::string> roomModelNames = { "room.fbx", "room1.fbx" };
std::string selectedRoomModel;

void saveGameState(const std::string& filepath, const std::vector<Model>& models, const std::string& selectedRoomModel);
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
    ImGuiIO& io = ImGui::GetIO();

    ImGui::SetNextWindowPos(ImVec2((io.DisplaySize.x - 600) * 0.5f, (io.DisplaySize.y - 100) * 0.5f));
    ImGui::SetNextWindowSize(ImVec2(600, 100));

    ImGui::Begin("Main Menu", &showMainMenu, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);

    // Make the "Interior Designer" text larger and horizontally centered
    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize("Interior Designer").x) * 0.5f);
    ImGui::Text("Interior Designer");

    ImGui::Spacing();

    float buttonWidth = 150.0f;
    float buttonHeight = 60.0f;

    // Calculate the horizontal offset to center the buttons
    float offset = (ImGui::GetWindowWidth() - 3 * buttonWidth - 2 * ImGui::GetStyle().ItemSpacing.x) * 0.5f;

    // Set the cursor position to the calculated offset
    ImGui::SetCursorPosX(offset);

    // Draw the buttons
    if (ImGui::Button("About", ImVec2(buttonWidth, buttonHeight))) {
        DisplaySecondaryWindow();
    }
    ImGui::SameLine();

    if (ImGui::Button("Start", ImVec2(buttonWidth, buttonHeight))) {
        DisplayChooseWindow();
    }
    ImGui::SameLine();

    if (ImGui::Button("Load Scene", ImVec2(buttonWidth, buttonHeight))) {
        std::string filepath = OpenFileDialog();
        if (!filepath.empty()) {
            // Handle button click and loading scene
            loadGameState(filepath, models, ourShader, selectedRoomModel);
            DisplayModelWindow();
        }
    }

    ImGui::End();
}

void SecondaryWindow() {
    ImGuiIO& io = ImGui::GetIO();

    ImGui::SetNextWindowPos(ImVec2((io.DisplaySize.x - 750) * 0.5f, (io.DisplaySize.y - 150) * 0.5f));
    ImGui::SetNextWindowSize(ImVec2(750, 150));

    ImGui::Begin("About", &showSecondaryWindow, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
    ImGui::Text("A simple interior designer program that lets you place some objects around the scene. Main controls:");
    ImGui::Separator();

    ImGui::Text("WASD - Movement");
    ImGui::Text("LMB + Mouse - Rotate the camera");
    ImGui::Text("Q - Save the scene");
    ImGui::Text("R - Open the Generate window");
    ImGui::Separator();

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
    // Get the display size to calculate the center position
    ImVec2 displaySize = ImGui::GetIO().DisplaySize;

    // Calculate the center position for the window
    ImVec2 windowSize(400, 200);
    ImVec2 windowPos((displaySize.x - windowSize.x) * 0.5f, (displaySize.y - windowSize.y) * 0.5f);

    ImGui::SetNextWindowPos(windowPos);
    ImGui::SetNextWindowSize(windowSize);

    ImGui::Begin("Choose a room", &showChooseWindow, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
    ImGui::Text("Choose the room preset");

    // Number of buttons
    int numButtons = roomModelNames.size();

    // Use the available width for the buttons
    float buttonWidth = 100.0f;  // Set your desired button width

    ImTextureID imguiTextureIDs[] = {
        reinterpret_cast<ImTextureID>(static_cast<intptr_t>(texture1)),
        reinterpret_cast<ImTextureID>(static_cast<intptr_t>(texture2)),
        reinterpret_cast<ImTextureID>(static_cast<intptr_t>(texture3)),
        reinterpret_cast<ImTextureID>(static_cast<intptr_t>(texture4))
    };

    // Check if there are enough textures for buttons
    assert(numButtons <= sizeof(imguiTextureIDs) / sizeof(imguiTextureIDs[0]));
    ImVec2 imageSize(100, 100);

    // Calculate total width occupied by buttons
    float totalButtonsWidth = numButtons * buttonWidth;

    // Calculate the offset to center buttons within the window
    float offsetX = (windowSize.x - totalButtonsWidth) * 0.5f;

    ImGui::SetCursorPosX(offsetX);
    ImGui::Image(imguiTextureIDs[0], imageSize);
    if (ImGui::Button("Preset #1")) {
        std::cout << "Clicked on Preset #1" << std::endl;
        showChooseWindow = false;
        showModelWindow = true;
        selectedRoomModel = roomModelNames[0];
    }
    ImGui::SameLine();  

    // Second button
    ImGui::SetCursorPosX(offsetX + buttonWidth + ImGui::GetStyle().ItemSpacing.x);
    ImGui::Image(imguiTextureIDs[1], imageSize);
    if (ImGui::Button("Preset #2")) {
        std::cout << "Clicked on Preset #2" << std::endl;
        showChooseWindow = false;
        showModelWindow = true;
        selectedRoomModel = roomModelNames[1];
    }
    ImGui::SameLine();  


    ImGui::End();
}


void initializeScene(Shader& ourShader,const char* texName,const std::string roomObj) {
    room = Model("resources/objects/" + roomObj,glm::vec3(0.0f,0.0f,0.0f),glm::vec3(0.0f,0.0f,0.0f),glm::vec3(1.0f,1.0f,1.0f));
    TextureFromFile(texName, "resources/objects");
    ourShader.use();
    glActiveTexture(GL_TEXTURE0);
}
void UpdateCamera(GLFWwindow* window, Camera& camera, bool ImGuiHandlingInput) {
    if (!ImGuiHandlingInput) {
        camera.updateMatrix(camera.zoom, 0.1f, 100.0f);
        camera.Inputs(window);
    }
}
void RenderGUI(int& selectedId, std::vector<Model>& models, std::vector<std::string>& modelNames) {
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
        // Sliders for changing position and rotation
        ImGui::SliderFloat("X Position", &models[selectedId].position.x, -30.0f, 30.0f);
        ImGui::SliderFloat("Y Position", &models[selectedId].position.y, -30.0f, 30.0f);
        ImGui::SliderFloat("Z Position", &models[selectedId].position.z, -30.0f, 30.0f);
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
}

void RenderModels(Shader& ourShader, const std::vector<Model>& models) {
    for (const Model& model : models) {
        if (!model.textures_loaded.empty()) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, model.textures_loaded[0].id);
            ourShader.setInt("texture_diffuse", 0);

        }
        glm::mat4 modelMatrix = model.GetTransformMatrix();
        ourShader.setMat4("model", modelMatrix);
        model.Draw(ourShader);
    }
}

void HandleInput(GLFWwindow* window, std::vector<Model>& models, Shader& outShader,int& selectedId) {
    // after clicking the R button show the list of objects to generate
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    {
        ImGui::OpenPopup("Generate");
    }

    if (ImGui::BeginPopup("Generate"))
    {
        if (ImGui::Button("Standard Chair")) {
            GenerateObject("chair1.fbx", "texture_diffuse1.jpg", ourShader, models.size(), posXYZ, glm::vec3(0.0f, glm::radians(rot), 0.0f), glm::vec3(1), "Standard Chair");
        }


        if (ImGui::Button("Dresser")) {
            GenerateObject("dresser.fbx", "texture_diffuse3.jpg", ourShader, models.size(), posXYZ, glm::vec3(0.0f, glm::radians(rot), 0.0f), glm::vec3(0.5), "Dresser");
        }

        if (ImGui::Button("Table")) {
            GenerateObject("table1.fbx", "texture_diffuse4.jpg", ourShader, models.size(), posXYZ, glm::vec3(0.0f, glm::radians(rot), 0.0f), glm::vec3(0.5), "Table");
        }

        if (ImGui::Button("Dresser2")) {
            GenerateObject("dresser2.fbx", "texture_diffuse5.jpg", ourShader, models.size(), posXYZ, glm::vec3(0.0f, glm::radians(rot), 0.0f), glm::vec3(0.5), "Dresser2");
        }
        if (ImGui::Button("Desk")) {
            GenerateObject("desk.fbx", "texture_diffuse6.jpg", ourShader, models.size(), posXYZ, glm::vec3(0.0f, glm::radians(rot), 0.0f), glm::vec3(0.5), "Desk");
        }
        if (ImGui::Button("Table2")) {
            GenerateObject("table2.fbx", "texture_diffuse1.jpg", ourShader, models.size(), posXYZ, glm::vec3(0.0f, glm::radians(rot), 0.0f), glm::vec3(0.), "Table2");
        }

        ImGui::EndPopup();
    }
    // Handle 'Q' key for Save
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        std::string filepath = SaveFileDialog();
        if (!filepath.empty()) {
            saveGameState(filepath, models,selectedRoomModel);
            std::cout << filepath << std::endl;
            std::cout << "Scene has been successfully saved to " << filepath << std::endl;
        }
    }
}
void RenderModelWindow(GLFWwindow* window, Shader& ourShader, std::vector<Model>& models, int& selectedId) {
    ourShader.use();
    ourShader.setMat4("camMatrix", camera.cameraMatrix);

    ourShader.setMat4("model", room.GetTransformMatrix());
    room.Draw(ourShader);

    ImGuiHandlingInput = ImGui::GetIO().WantCaptureMouse;
    ImGui::Begin("Viewport", &showModelWindow);

    for (auto& name : modelNames) {
        if (name.empty()) {
            std::cerr << "ERROR: empty model name!" << std::endl;
            name = "none";
        }
    }
    UpdateCamera(window, camera, ImGuiHandlingInput);
    RenderGUI(selectedId, models, modelNames);
    HandleInput(window, models, ourShader, selectedId);
    RenderModels(ourShader, models);

    ImGui::End();
}
void saveGameState(const std::string& filepath, const std::vector<Model>& models, const std::string& selectedRoomModel) {
    std::ofstream outFile(filepath, std::ios::binary);
    if (!outFile) {
        throw std::runtime_error("Failed to open file for saving");
    }

    // Serialize the selected room model
    size_t roomModelLength = selectedRoomModel.length();
    outFile.write(reinterpret_cast<const char*>(&roomModelLength), sizeof(roomModelLength));
    outFile.write(selectedRoomModel.c_str(), roomModelLength);

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

    // Deserialize the selected room model
    size_t roomModelLength;
    inFile.read(reinterpret_cast<char*>(&roomModelLength), sizeof(roomModelLength));
    char* roomModelBuffer = new char[roomModelLength + 1];
    inFile.read(roomModelBuffer, roomModelLength);
    roomModelBuffer[roomModelLength] = '\0';
    selectedRoomModel = std::string(roomModelBuffer);
    delete[] roomModelBuffer;

    models.clear(); // Clear existing models

    while (inFile.peek() != EOF) {
        ModelSnapshot snapshot;
        snapshot.deserialize(inFile);

        Model model("resources/objects/"+snapshot.objectName, snapshot.position, snapshot.rotation, snapshot.scale);
        model.setPosition(snapshot.position);
        model.setRotation(snapshot.rotation);
        model.setScale(snapshot.scale);
        model.objectName = snapshot.objectName;
        model.textureName = snapshot.textureName;

        if (!model.objectName.empty()) {
            modelNames.push_back(model.objectName);
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
            texture.id = TextureFromFile(textureSnapshot.path.c_str(), "resources/objects/");
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

    // query the primary monitor's video mode
    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);

    // set the window size to match the monitor resolution
    const int monitorWidth = mode->width;
    const int monitorHeight = mode->height;

    GLFWwindow* window = glfwCreateWindow(monitorWidth, monitorHeight, "Interior Designer", NULL, NULL);
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

    const double targetFrameTime = 1.0 / 60.0; // 60 fps
    double lastFrameTime = glfwGetTime();
    double lastFPSUpdateTime = lastFrameTime; // Initialize lastFPSUpdateTime
    int frameCount = 0;

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();  // Process GLFW events

        // Calculate elapsed time since the last frame
        double currentFrameTime = glfwGetTime();
        double deltaTime = currentFrameTime - lastFrameTime;

        if (deltaTime >= targetFrameTime)
        {
            // Update last frame time
            lastFrameTime = currentFrameTime;

            frameCount++;

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
                initializeScene(ourShader, "texture_diffuse2.jpg", selectedRoomModel);
                RenderModelWindow(window, ourShader, models,selectedId);
            }

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(window);
        }
        // Calculate FPS every second
        if (currentFrameTime - lastFPSUpdateTime >= 1.0)
        {
            double fps = frameCount / (currentFrameTime - lastFPSUpdateTime);
            std::cout << "FPS: " << fps << std::endl;

            // Reset frame count and update last FPS update time
            frameCount = 0;
            lastFPSUpdateTime = currentFrameTime;
        }
    }

    // delete all resources
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}

