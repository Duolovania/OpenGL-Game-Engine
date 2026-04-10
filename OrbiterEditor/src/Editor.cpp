#include "editor.h"
#include "tinyfiledialogs/tinyfiledialogs.h"
#include <cmath>
#include <filesystem>

FileManager fileManager;
int selectedObject = -2, startCameraIndex = -1;
std::string inputString, searchTerm, assetsPath, currentPath, defaultScriptPath;

float sprintSpeed;
float iconSize = 200;

float rectangleVertices[] =
{
    // Coords    // texCoords
     1.0f, -1.0f,  1.0f, 0.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
    -1.0f,  1.0f,  0.0f, 1.0f,

     1.0f,  1.0f,  1.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
    -1.0f,  1.0f,  0.0f, 1.0f
};

enum UISelect
{
    None,
    ActionButton,
    KeybindButton
};

UISelect uiSelect = UISelect::None;

std::shared_ptr<FrameBuffer> editorFB, previewFB;

std::unique_ptr<Texture> iconTextures;
GLuint64 playButton, pauseButton, stopButton, fileIcon, folderIcon, wavFileIcon, fontFileIcon, sceneFileIcon, imageFileIcon, miniFolderIcon, resetIcon, scriptIcon;

void Editor::Init(GLFWwindow* window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    StylesConfig();

    // Creates frame buffer shader.
    fbShader = std::make_unique<Shader>("res/Shaders/Framebuffer.shader");
    fbShader->CreateShader();
    fbShader->Bind();

    Core.renderer.Init(); // Generates essential rendering data.

    fbShader->Bind();
    fbShader->SetUniform1i("screenTexture", 1);

    // Generates and binds the frame buffer vertex array.
    frameBufferVA = std::make_unique<VertexArray>();
    frameBufferVA->Gen();
    frameBufferVA->Bind();

    // Generates the frame buffer vertex buffer.
    frameBufferVB = std::make_unique<VertexBuffer>(sizeof(rectangleVertices));
    frameBufferVB->Gen(&rectangleVertices);
    
    // Defines the vertex buffer layout.
    VertexBufferLayout layout;
    layout.Push<float>(2);
    layout.Push<float>(2);

    frameBufferVA->AddBuffer(*frameBufferVB, layout); // Adds the vertex buffer to the vertex array.

    // Copies main framebuffer screen width/height.
    editorFB = std::make_unique<FrameBuffer>(framebuffer->GetSize().x, framebuffer->GetSize().y);
    editorFB->Gen();

    previewFB = std::make_unique<FrameBuffer>(framebuffer->GetSize().x, framebuffer->GetSize().y);
    previewFB->Gen();

    editorCameraObj.transform.position = glm::vec3(0, 0, 0);
    editorCameraObj.transform.rotation = glm::vec3(0, 0, 0);

    // Sets the project 'Assets' folder path to the corresponding project directory using the project name.
    assetsPath = Core.selectedProject.assetsFolderPath;
    currentPath = assetsPath;

    iconTextures = std::make_unique<Texture>("res/Application Icons/playbutton.png");

    // Loads the editor application icons (e.g. play button, folder icon, reset properties icon, etc.)
    playButton = iconTextures->Load("res/Application Icons/playbutton.png", true);
    pauseButton = iconTextures->Load("res/Application Icons/pausebutton.png", true);
    stopButton = iconTextures->Load("res/Application Icons/stopbutton.png", true);

    folderIcon = iconTextures->Load("res/Application Icons/foldericon.png", true);
    fileIcon = iconTextures->Load("res/Application Icons/fileicon.png", true);
    scriptIcon = iconTextures->Load("res/Application Icons/scriptfileicon.png", true);

    wavFileIcon = iconTextures->Load("res/Application Icons/wavfileicon.png", true);
    fontFileIcon = iconTextures->Load("res/Application Icons/fontfileicon.png", true);
    sceneFileIcon = iconTextures->Load("res/Application Icons/scenefileicon.png", true);
    imageFileIcon = iconTextures->Load("res/Application Icons/imagefileicon.png", true);
    miniFolderIcon = iconTextures->Load("res/Application Icons/foldericon - mini.png", true);
    resetIcon = iconTextures->Load("res/Application Icons/reset.png", true);

    defaultScriptPath = "res/Copy Files/defaultscript.lua";

    // Sets the first scene to a new, empty scene.
    currentScene.sceneName = "Untitled";
    currentScene.objectsToRender.clear();

    Camera defaultCamera;
    editorCameraObj.AddComponent(defaultCamera);
    gameCameraObj.AddComponent(defaultCamera);
    previewCameraObj.AddComponent(defaultCamera);

    // Updates the renderer data.
    Core.renderer.objectsToRender = currentScene.objectsToRender;
    Core.renderer.RegenerateObjects();

    // Sets the editor configuration.
    selectedEditorConfig = fileManager.LoadEditorConfig(Core.selectedProject.projectFolderPath + "\\default.editorOB");

    // Shows the console window if it was turned on before. By default, it is turned off in 'Application.cpp', so toggling it will turn it on.
    if (selectedEditorConfig.showConsoleWindow) Core.ToggleConsoleWindow();
}

bool Editor::OnUpdate(float deltaTime, float time)
{
    // Creates ImGui window frames.
    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();

    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_None);

    MenuBar(); // Shows the navigation bar at the top of the screen.

    ContentBrowser(); // Shows the assets folder window.
    if (!applicationRunning) return applicationRunning;
    Inspector(); // Shows the inspector window.

    Hierarchy(); // Shows the hierarchy window.
    Viewport(); // Shows the viewport window.
    GameView(); // Shows the game view window.

    // Checks if the screen has been minimized.
    if (viewportSize.x < 0 || viewportSize.y < 0)
    {
        ImGui::Render();
        return true;
    }

    // Game camera.
    framebuffer->Bind();

    Camera gameCamera = *gameCameraObj.GetComponent<Camera>();

    // Copies the main camera view.
    if (startCameraIndex > -1)
    {
        gameCameraObj.transform = Core.renderer.objectsToRender[startCameraIndex].transform;
        gameCamera = *Core.renderer.objectsToRender[startCameraIndex].GetComponent<Camera>();
    }

    GLCall(glClearColor(gameCamera.backgroundColor[0], gameCamera.backgroundColor[1], gameCamera.backgroundColor[2], 1.0f));
    GLCall(glClear(GL_COLOR_BUFFER_BIT));

    sprintSpeed = Project.InputManager.GetActionStrength("sprint") * 150; // Camera movement sprint speed.

    // Game camera properties.
    gameCameraObj.transform.scale = glm::vec3(viewportSize.x, viewportSize.y, 0);

    // Render scene objects.
    Core.renderer.Draw(glm::ortho(((float)viewportSize.x / (float)viewportSize.y) * -100, ((float)viewportSize.x / (float)viewportSize.y) * 100, -100.0f, 100.0f, -1.0f, 1.0f), gameCameraObj.GetView(), { gameCamera.outputColor[0], gameCamera.outputColor[1], gameCamera.outputColor[2], gameCamera.outputColor[3] });

    DebugWindow(); // Shows the debug console window.
    OptionalWindows(); // Shows additional windows (e.g. rendering stats, project settings, etc.)

    // Unbinds frame buffer components.
    framebuffer->UnBind();

    // Editor camera.
    editorFB->Bind();

    Camera editorCamera = *editorCameraObj.GetComponent<Camera>();

    GLCall(glClearColor(editorCamera.backgroundColor[0], editorCamera.backgroundColor[1], editorCamera.backgroundColor[2], 1.0f));
    GLCall(glClear(GL_COLOR_BUFFER_BIT));

    // Editor camera properties.
    editorCameraObj.transform.scale = glm::vec3(viewportSize.x, viewportSize.y, 0);
    editorCameraObj.transform.position += glm::vec3(Project.InputManager.BasicMovement().x * (100.0f + sprintSpeed) * deltaTime, Project.InputManager.BasicMovement().y * (100.0f + sprintSpeed) * deltaTime, 0);

    // Render scene objects.
    Core.renderer.Draw(glm::ortho(((float)viewportSize.x / (float)viewportSize.y) * -100, ((float)viewportSize.x / (float)viewportSize.y) * 100, -100.0f, 100.0f, -1.0f, 1.0f), editorCameraObj.GetView(), { editorCamera.outputColor[0], editorCamera.outputColor[1], editorCamera.outputColor[2], editorCamera.outputColor[3] });

    DebugWindow(); // Shows the debug console window.
    OptionalWindows(); // Shows additional windows (e.g. rendering stats, project settings, etc.)

    // Unbinds frame buffer components.
    editorFB->UnBind();

    // Camera preview.
    previewFB->Bind();

    Camera previewCamera = *previewCameraObj.GetComponent<Camera>();

    GLCall(glClearColor(previewCamera.backgroundColor[0], previewCamera.backgroundColor[1], previewCamera.backgroundColor[2], 1.0f));
    GLCall(glClear(GL_COLOR_BUFFER_BIT));

    // Preview camera properties.
    previewCameraObj.transform.scale = glm::vec3(viewportSize.x, viewportSize.y, 0);

    // Render scene objects.
    Core.renderer.Draw(glm::ortho(((float)viewportSize.x / (float)viewportSize.y) * -100, ((float)viewportSize.x / (float)viewportSize.y) * 100, -100.0f, 100.0f, -1.0f, 1.0f), previewCameraObj.GetView(), { previewCamera.outputColor[0], previewCamera.outputColor[1], previewCamera.outputColor[2], previewCamera.outputColor[3] });

    DebugWindow(); // Shows the debug console window.
    OptionalWindows(); // Shows additional windows (e.g. rendering stats, project settings, etc.)

    // Unbinds frame buffer components.
    previewFB->UnBind();

    fbShader->Bind();

    // Renders ImGui data.
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    return applicationRunning; // Returns the state of the application.
}

void Editor::Close()
{
    // Updates the editor config.
    fileManager.CreateEditorConfig(selectedEditorConfig, Core.selectedProject.projectFolderPath + "\\default.editorOB");

    // Updates the project config.
    fileManager.CreateProjectConfig(Core.selectedProject, Core.selectedProject.projectFilePath);

    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
}

void Editor::StylesConfig()
{
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    io.FontDefault = io.Fonts->AddFontFromFileTTF("res/Fonts/open-sans/OpenSans-Semibold.ttf", 18.0f);    
    pixelFont = io.Fonts->AddFontFromFileTTF("res/Fonts/joystix/joystix monospace.otf", 10.0f);

    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGuiStyle& style = ImGui::GetStyle();

    style.WindowRounding = 3.5f;
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.055f, 0.055f, 0.055f, 1.0f);

    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.075f, 0.075f, 0.075f, 1.0f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.055f, 0.055f, 0.055f, 1.0f);

    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.075f, 0.075f, 0.075f, 1.0f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.118f, 0.118f, 0.118f, 1.0f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.055f, 0.055f, 0.055f, 1.0f);

    style.Colors[ImGuiCol_Button] = ImVec4(0.075f, 0.075f, 0.075f, 1.0f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.135f, 0.28f, 0.135f, 1.0f);

    style.Colors[ImGuiCol_Header] = ImVec4(0.075f, 0.075f, 0.075f, 1.0f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.055f, 0.055f, 0.055f, 1.0f);

    style.Colors[ImGuiCol_DockingPreview] = ImVec4(0.141f, 0.729f, 0.38f, 1.0f);

    style.Colors[ImGuiCol_TabDimmedSelected] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
    style.Colors[ImGuiCol_TabHovered] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);

    style.Colors[ImGuiCol_Tab] = ImVec4(0.075f, 0.075f, 0.075f, 1.0f);
    style.Colors[ImGuiCol_TabSelected] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
    style.Colors[ImGuiCol_TabSelectedOverline] = ImVec4(0.135f, 0.28f, 0.135f, 1.0f);
    style.Colors[ImGuiCol_TabDimmedSelectedOverline] = ImVec4(0.075f, 0.075f, 0.075f, 1.0f);

    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.135f, 0.28f, 0.135f, 1.0f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.177f, 0.479f, 0.177f, 1.0f);

    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.135f, 0.28f, 0.135f, 1.0f);

    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.318f, 0.318f, 0.318f, 1.0f);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.697f, 0.0f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.469f, 0.469f, 0.469f, 1.0f);
}

void Editor::Viewport()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("Viewport");
    AddTooltip("This window provides a preview of the scene for editing purposes."); // Add tooltip for UI element above.

    viewportSize = ImGui::GetContentRegionAvail();
    if (editorFB->GetSize() != glm::vec2(viewportSize.x, viewportSize.y)) editorFB->Resize(glm::vec2(viewportSize.x, viewportSize.y));

    ImGui::Image((void*)editorFB->GetTexture(), ImVec2(editorFB->GetSize().x, editorFB->GetSize().y), ImVec2(0, 1), ImVec2(1, 0));
    ImVec2 imagePos = ImGui::GetCursorScreenPos();

    ImVec2 position = ImVec2(imagePos.x * 1.15f, imagePos.y / 10.15f);
    ImGui::SetCursorScreenPos(position);

    // Checks if the FPS count should be shown.
    if (selectedEditorConfig.showFPS)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.988f, 0.659f, 0.176f, 1.0f));
        ImGui::Text("FPS: %.1f", double(1.0f / ImGui::GetIO().DeltaTime));
        ImGui::PopStyleColor();
    }

    ImGui::End();
    ImGui::PopStyleVar();
}

void Editor::GameView()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("Game View");
    AddTooltip("This is how your game will look!"); // Add tooltip for UI element above.

    viewportSize = ImGui::GetContentRegionAvail();
    if (framebuffer->GetSize() != glm::vec2(viewportSize.x, viewportSize.y)) framebuffer->Resize(glm::vec2(viewportSize.x, viewportSize.y));

    ImGui::Image((void*)framebuffer->GetTexture(), ImVec2(framebuffer->GetSize().x, framebuffer->GetSize().y), ImVec2(0, 1), ImVec2(1, 0));
    ImVec2 imagePos = ImGui::GetCursorScreenPos();

    ImVec2 position = ImVec2(imagePos.x * 1.15f, imagePos.y / 10.15f);
    ImGui::SetCursorScreenPos(position);

    // Checks if the FPS count should be shown.
    if (selectedEditorConfig.showFPS)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.988f, 0.659f, 0.176f, 1.0f));
        ImGui::Text("FPS: %.1f", double(1.0f / ImGui::GetIO().DeltaTime));
        ImGui::PopStyleColor();
    }

    ImGui::End();
    ImGui::PopStyleVar();
}

// Recursively displays hierarchy.
void ShowHierarchy(const std::string& name, int index, bool isLeaf = false) 
{
    // Check if the node does not contain other objects.
    if (isLeaf) 
    {
        // Shows a 'regular' button.
        if (ImGui::Selectable(name.c_str(), false)) 
        {
            selectedObject = index; // Updates the selected object in the hierarchy.
        }
    }
    else 
    {
        if (ImGui::TreeNode(name.c_str())) 
        {
            selectedObject = index; // Updates the selected object in the hierarchy.

            ShowHierarchy("Child 1", 0, true);
            ImGui::TreePop(); // Pops the style to prevent style changes to other tree nodes.
        }
    }
}

void Editor::Hierarchy()
{
    ImGui::Begin("Hierarchy");
    AddTooltip("This window shows a list of all objects in the scene."); // Add tooltip for UI element above.

    if (ImGui::Button("Add"))
    {
        GameObject newGObj;
        newGObj.transform.position = glm::vec3();

        Core.renderer.objectsToRender.push_back(newGObj);
        Core.renderer.RegenerateObject(Core.renderer.objectsToRender.size() - 1);
    }
    AddTooltip("Adds a blank GameObject to the scene."); // Add tooltip for UI element above.

    for (int i = 0; i < Core.renderer.objectsToRender.size(); i++)
    {
        ImGui::PushID(i);
        ShowHierarchy(Core.renderer.objectsToRender[i].objectName, i, true);
        ImGui::PopID();
    }

    ImGui::End();
}

void Editor::Inspector()
{
    ImGui::Begin("Inspector");
    AddTooltip("This window covers the details for each component in the GameObject."); // Add tooltip for UI element above.

    if (selectedObject >= 0 && Core.renderer.objectsToRender.size() >= 1)
    {
        ImGui::InputText("##label0", &Core.renderer.objectsToRender[selectedObject].objectName);

        ImGui::SameLine();

        if (ImGui::Button("Jump To"))
        {
            editorCameraObj.transform.position = Core.renderer.objectsToRender[selectedObject].transform.position;
        }
        AddTooltip("See where this object is located on the scene."); // Add tooltip for UI element above.

        // Check that there is more than 1 object in the scene.
        if (Core.renderer.objectsToRender.size() > 1)
        {
            ImGui::SameLine();

            if (ImGui::Button("Delete"))
            {
                Core.renderer.objectsToRender.erase(Core.renderer.objectsToRender.begin() + selectedObject);
                selectedObject = 0; // Set the default selected item. 
            }
            AddTooltip("Delete the object from the scene."); // Add tooltip for UI element above.
        }

        if (ImGui::CollapsingHeader("Transform"))
        {
            if (ImGui::BeginTable("TransformTable", 4))
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);

                ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x / 4); // Set the width of each input field
                ImGui::Text("Position:");
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.3f, 0.3f, 1.0f)); // X color

                ImGui::TableSetColumnIndex(1);
                ImGui::Text("X");

                ImGui::SameLine();
                ImGui::SetCursorPosY(ImGui::GetCursorPos().y - 3);

                ImGui::InputFloat("##PX", &Core.renderer.objectsToRender[selectedObject].transform.position.x, 0.0f, 0.0f, "%.f");
                ImGui::PopStyleColor();

                ImGui::TableSetColumnIndex(2);
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 1.0f, 0.3f, 1.0f)); // Y color

                ImGui::SetCursorPosY(ImGui::GetCursorPos().y - 3);
                ImGui::Text("Y");

                ImGui::SameLine();
                ImGui::InputFloat("##PY", &Core.renderer.objectsToRender[selectedObject].transform.position.y, 0.0f, 0.0f, "%.f");
                ImGui::PopStyleColor();

                ImGui::TableSetColumnIndex(3);

                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 0.3f, 1.0f, 1.0f)); // Z color

                ImGui::SetCursorPosY(ImGui::GetCursorPos().y - 3);
                ImGui::Text("Z");

                ImGui::SameLine();
                ImGui::InputFloat("##PZ", &Core.renderer.objectsToRender[selectedObject].transform.position.z, 0.0f, 0.0f, "%.f");
                ImGui::PopStyleColor();


                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);

                ImGui::Text("Rotation:");

                ImGui::TableSetColumnIndex(1);
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.3f, 0.3f, 1.0f)); // X color

                ImGui::Text("X");

                ImGui::SameLine();
                ImGui::SetCursorPosY(ImGui::GetCursorPos().y - 3);

                ImGui::InputFloat("##RX", &Core.renderer.objectsToRender[selectedObject].transform.rotation.x, 0.0f, 0.0f, "%.f");
                ImGui::PopStyleColor();

                ImGui::TableSetColumnIndex(2);

                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 1.0f, 0.3f, 1.0f)); // Y color

                ImGui::SetCursorPosY(ImGui::GetCursorPos().y - 3);
                ImGui::Text("Y");

                ImGui::SameLine();
                ImGui::InputFloat("##RY", &Core.renderer.objectsToRender[selectedObject].transform.rotation.y, 0.0f, 0.0f, "%.f");
                ImGui::PopStyleColor();

                ImGui::TableSetColumnIndex(3);

                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 0.3f, 1.0f, 1.0f)); // Z color

                ImGui::SetCursorPosY(ImGui::GetCursorPos().y - 3);
                ImGui::Text("Z");

                ImGui::SameLine();
                ImGui::InputFloat("##RZ", &Core.renderer.objectsToRender[selectedObject].transform.rotation.z, 0.0f, 0.0f, "%.f");
                ImGui::PopStyleColor();

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);

                ImGui::Text("Scale:");
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.3f, 0.3f, 1.0f)); // X color

                ImGui::TableSetColumnIndex(1);

                ImGui::Text("X");

                ImGui::SameLine();
                ImGui::SetCursorPosY(ImGui::GetCursorPos().y - 3);

                ImGui::InputFloat("##SX", &Core.renderer.objectsToRender[selectedObject].transform.scale.x, 0.0f, 0.0f, "%.f");
                ImGui::PopStyleColor();

                ImGui::TableSetColumnIndex(2);

                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 1.0f, 0.3f, 1.0f)); // Y color

                ImGui::SetCursorPosY(ImGui::GetCursorPos().y - 3);
                ImGui::Text("Y");

                ImGui::SameLine();
                ImGui::InputFloat("##SY", &Core.renderer.objectsToRender[selectedObject].transform.scale.y, 0.0f, 0.0f, "%.f");
                ImGui::PopStyleColor();

                ImGui::TableSetColumnIndex(3);

                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 0.3f, 1.0f, 1.0f)); // Z color

                ImGui::SetCursorPosY(ImGui::GetCursorPos().y - 3);
                ImGui::Text("Z");

                ImGui::SameLine();
                ImGui::InputFloat("##SZ", &Core.renderer.objectsToRender[selectedObject].transform.scale.z, 0.0f, 0.0f, "%.f");
                ImGui::PopStyleColor();

                ImGui::PopItemWidth(); // Reset item width

                /*std::array<Vector3, 3> thigny = { renderer.objectsToRender[selectedObject].transform.position, renderer.objectsToRender[selectedObject].transform.rotation, renderer.objectsToRender[selectedObject].transform.scale };
                CreateTransformColumn({ "Position", "Rotation", "Scale" }, thigny);*/

                ImGui::EndTable();
            }
        }

        // Checks if the object has a sprite renderer component before showing the dropdown.
        if (Core.renderer.objectsToRender[selectedObject].HasComponent("Sprite Renderer"))
        {
            if (ImGui::CollapsingHeader("Sprite Renderer"))
            { 
                SpriteRenderer spriteRenderer = *Core.renderer.objectsToRender[selectedObject].GetComponent<SpriteRenderer>();

                ImGui::Image((void*)spriteRenderer.cTexture.textureBuffer, ImVec2(200, 200), ImVec2(0, 1), ImVec2(1, 0));

                ImGui::SameLine();

                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 1.0f, 0.3f, 0.5f)); // Sets the text to green.

                std::string imageName = spriteRenderer.cTexture.m_imagePath.erase(0, spriteRenderer.cTexture.m_imagePath.find_last_of("/") + 1); // Cuts the image directory, leaving just the name and extension.
                ImGui::Text(imageName.c_str());

                ImGui::PopStyleColor(); // Pops the style.

                ImGui::SameLine();

                // Creates a button that allows the user to pick an image file. 
                if (ImGui::ImageButton((void*)miniFolderIcon, ImVec2(20, 20), ImVec2(0, 1), ImVec2(1, 0)))
                {
                    const char* filterTypes[4] = { "*.png", "*.jpg"};
                    const char* file_path = tinyfd_openFileDialog(
                        "Open an image file",              // Title of the dialog
                        (assetsPath + "/Assets").c_str(),                         // Default path ("" means current directory)
                        2,                          // Number of file filters
                        filterTypes,                // File filters (e.g., ["*.txt"])
                        "Image",                       // Filter description
                        0                           // Allow multiple selections (0 for no)
                    );

                    // Checks if the file exists.
                    if (file_path)
                    {
                        spriteRenderer.cTexture.m_imagePath = assetsPath + std::string(file_path).erase(0, assetsPath.length()); // Erases the directories leading up to the "Assets" folder.
                        Core.renderer.RegenerateObject(selectedObject); // Updates the image to apply changes.
                    }
                }
                AddTooltip("Locate the file through your files window."); // Adds a tooltip to the UI element above.

                ImGui::Text("Colour:");
                ImGui::SameLine();
                ImGui::ColorEdit4("##label4", (float*)&spriteRenderer.color);

                ImGui::SameLine();

                if (ImGui::ImageButton((void*) resetIcon, ImVec2(20, 20), ImVec2(0, 1), ImVec2(1, 0)))
                {
                    spriteRenderer.SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
                }
                AddTooltip("Reset to default."); // Adds a tooltip to the UI element above.
            }
        }
        else
        {
            // Check if the object does not have a camera component before providing the option to add a sprite renderer.
            if (selectedObject >= 0 && !Core.renderer.objectsToRender[selectedObject].HasComponent("Camera"))
            {
                if (ImGui::Button("Add Sprite Renderer"))
                {
                    SpriteRenderer spriteRendererComponent;
                    Core.renderer.objectsToRender[selectedObject].AddComponent(spriteRendererComponent);
                }
            }
        }

        // Checks if the object has a camera component before showing the dropdown.
        if (Core.renderer.objectsToRender[selectedObject].HasComponent("Camera"))
        {
            previewCameraObj = Core.renderer.objectsToRender[selectedObject]; // Sets this gameobject as the preview.

            if (ImGui::CollapsingHeader("Camera"))
            {
                // BAZINGA
                Camera *camera = &*Core.renderer.objectsToRender[selectedObject].GetComponent<Camera>();

                ImGui::Image((void*)previewFB->GetTexture(), ImVec2(200, 200), ImVec2(0, 1), ImVec2(1, 0));

                ImGui::Text("Output Colour:");
                ImGui::SameLine();
                ImGui::ColorEdit4("##label7", (float*)&camera->outputColor);

                ImGui::SameLine();
                if (ImGui::ImageButton((void*)resetIcon, ImVec2(20, 20), ImVec2(0, 1), ImVec2(1, 0)))
                {
                    camera->SetColor(camera->outputColor, { 1.0f, 1.0f, 1.0f, 1.0f });
                }

                ImGui::Text("Background Colour:");
                ImGui::SameLine();
                ImGui::ColorEdit3("##label8", (float*)&camera->backgroundColor);

                ImGui::SameLine();
                if (ImGui::ImageButton((void*)resetIcon, ImVec2(20, 20), ImVec2(0, 1), ImVec2(1, 0)))
                {
                    camera->SetColor(camera->backgroundColor, { 0.05f, 0.05f, 0.05f, 1.0f });
                }
                AddTooltip("Reset to default."); // Adds a tooltip to the UI element above.

                if (ImGui::Button("Set as Starting Camera"))
                {
                    // Starting camera logic.
                    startCameraIndex = selectedObject;
                }
                AddTooltip("Set this camera to be the 'main camera.'"); // Adds a tooltip to the UI element above.

                // Checks if this is the starting camera.
                if (startCameraIndex == selectedObject)
                {
                    ImGui::SameLine();
                    ImGui::Text("[This is the starting camera!]");
                }
            }
        }
        else
        {
            // Check if the object does not have a sprite renderer component before providing the option to add a camera.
            if (selectedObject >= 0 && !Core.renderer.objectsToRender[selectedObject].HasComponent("Sprite Renderer"))
            {
                if (ImGui::Button("Add Camera"))
                {
                    Camera cameraComponent;
                    Core.renderer.objectsToRender[selectedObject].AddComponent(cameraComponent);
                }
            }
        }

        // Checks if the object has an audio manager component before showing the dropdown.
        if (Core.renderer.objectsToRender[selectedObject].HasComponent("Audio Manager"))
        {
            if (ImGui::CollapsingHeader("Audio Manager"))
            {
                AudioManagerComponent();
            }
        }
        else
        {
            // Check an object is selected before providing the option to add an audio manager.
            if (selectedObject >= 0)
            {
                if (ImGui::Button("Add Audio Manager"))
                {
                    AudioManager audioManagerComponent;
                    Core.renderer.objectsToRender[selectedObject].AddComponent(audioManagerComponent);
                }
            }
        }

        // Checks if the object has a script component before showing the dropdown.
        if (Core.renderer.objectsToRender[selectedObject].HasComponent("Script Manager"))
        {
            if (ImGui::CollapsingHeader("Script Manager"))
            {
                ScriptManagerComponent();
            }
        }
        else
        {
            // Check an object is selected before providing the option to add a script.
            if (selectedObject >= 0)
            {
                if (ImGui::Button("Add Script Manager"))
                {
                    ScriptManager scriptManagerComponent;
                    Core.renderer.objectsToRender[selectedObject].AddComponent(scriptManagerComponent);
                }
            }
        }
    }

    ImGui::End();
}

// Checks if the current directory has sub-folders.
bool HasSubFolder(const std::filesystem::path& folderPath)
{
    for (const auto& entry : std::filesystem::directory_iterator(folderPath))
    {
        if (entry.is_directory())
        {
            return true;
        }
    }

    return false;
}

// Creates a selectable for each folder in Assets.
void ShowFolders(const std::filesystem::path& folderPath, bool isLeaf = false) 
{
        for (const auto& entry : std::filesystem::directory_iterator(folderPath))
        {
            bool isFolderOpen = false;

            if (entry.is_directory())
            {
                if (HasSubFolder(entry))
                {
                    if (ImGui::TreeNode("##label"))
                    {
                        isFolderOpen = true;
                        ImGui::SameLine();

                        if (ImGui::Selectable(entry.path().filename().string().c_str(), false)) currentPath = std::string(entry.path().string());

                        // Recursively display subfolders
                        ShowFolders(entry.path(), true);
                        ImGui::TreePop();
                    }

                    if (!isFolderOpen)
                    {
                        ImGui::SameLine();

                        if (ImGui::Selectable(entry.path().filename().string().c_str(), false)) currentPath = std::string(entry.path().string());
                    }
                }
                else
                {
                    if (ImGui::Selectable(entry.path().filename().string().c_str(), false))
                    {
                        currentPath = std::string(entry.path().string());
                    }
                }
            }
        }
}

// Draws the assets folder window.
void Editor::ContentBrowser()
{
    // Check if the "Assets" folder exists in the project path.
    if (!std::filesystem::is_directory(assetsPath))
    {
        ImGui::OpenPopup("EDITOR ERROR");

        if (ImGui::BeginPopupModal("EDITOR ERROR", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Could not find the 'Assets' folder path for the Assets window.");

            // Exit editor option.
            if (ImGui::Button("Close Editor"))
            {
                applicationRunning = false;
                return;

                ImGui_ImplGlfw_Shutdown();
                ImGui_ImplOpenGL3_Shutdown();
                ImGui::DestroyContext();
            }

            ImGui::SameLine();
            // Sets clipboard to the folder path.
            if (ImGui::Button("Copy Folder Path")) 
            {
                const char* value_to_copy = assetsPath.c_str();
                ImGui::SetClipboardText(value_to_copy);
            }
        }

        ImGui::EndPopup();
    }
    else
    {
        ImGui::Begin("Assets Folder");
        std::string toolTipMsg = "This is a repository for all your game assets. This can be found at: " + assetsPath;
        AddTooltip(toolTipMsg.c_str()); // Add tooltip for UI element above.

        ImGui::BeginChild("TableChild", ImVec2(0, ImGui::GetContentRegionAvail().y), true, ImGuiWindowFlags_HorizontalScrollbar);

        if (ImGui::BeginTable("FolderTable", 2, ImGuiTableFlags_BordersInner | ImGuiTableFlags_RowBg))
        {
            ImGui::TableSetupColumn("List", ImGuiTableColumnFlags_WidthFixed, ImGui::GetWindowContentRegionMax().x * 0.2f); // Set width
            ImGui::TableSetupColumn("Icon", ImGuiTableColumnFlags_WidthFixed, ImGui::GetContentRegionAvail().x);

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);

            if (ImGui::BeginChild("Folder List Items"))
            {
                ShowFolders(assetsPath);

                ImGui::EndChild();
            }

            ImGui::TableSetColumnIndex(1);

            ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + 10, ImGui::GetCursorPos().y));

            if (ImGui::Button("<"))
            {
                std::filesystem::path parentDir = currentPath;
                currentPath = parentDir.parent_path().string();

                if (currentPath.length() < assetsPath.length()) currentPath = assetsPath;
            }

            ImGui::SameLine();

            ImGui::Text("Search");
            AddTooltip("Search for an item in the folder."); // Add tooltip for UI element above.

            ImGui::SameLine();
            ImGui::InputText("##search", &searchTerm);

            ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + 10, ImGui::GetCursorPos().y));

            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.224, 0.482, 0.267, 1.0f));

            std::string tempPath = currentPath;
            std::string selectedPath = tempPath.erase(0, assetsPath.length());

            ImGui::Text(selectedPath.c_str());
            ImGui::PopStyleColor();

            if (ImGui::BeginChild("FolderItems"))
            {
                int counter = 0;
                ImVec2 padding = ImVec2(iconSize * 0.125f, iconSize * 0.125f);
                ImVec2 originalPos = ImVec2(ImGui::GetCursorPos().x + padding.x, ImGui::GetCursorPos().y + padding.y);

                for (const auto& entry : std::filesystem::directory_iterator(currentPath))
                {
                    ImVec2 buttonSize = ImVec2(iconSize, iconSize);
                    ImVec2 buttonPos = ImVec2(originalPos.x + (counter * (buttonSize.x + padding.x)), originalPos.y); // Calculates the x position based on how many items there are.

                    ImGui::SetWindowFontScale(buttonSize.x / 195.0f);

                    // Sets the text position to the center of the thumbnail (sets the text origin position to the center of the thumbnail and subtracts it by the amount of characters. The subtraction is to ensure that the text is centered regardless of it's length).
                    ImVec2 textPos = ImVec2(buttonPos.x + (buttonSize.x - ImGui::CalcTextSize(entry.path().filename().string().c_str()).x) / 2, originalPos.y + (buttonSize.y + padding.y));

                    // Checks if the thumbnail will exceed the window size.
                    if ((buttonPos.x + buttonSize.x > ImGui::GetContentRegionMax().x))
                    {
                        counter = 0; // Resets the x position.
                        originalPos = ImVec2(originalPos.x, ImGui::GetCursorPos().y + ((buttonSize.y / 4) + padding.y)); // Calculates the y position based on the button size and padding amount.

                        buttonPos = ImVec2(originalPos.x + (counter * (buttonSize.x + padding.x)), originalPos.y); // Recalculates the button position with the x position being reset.
                        textPos = ImVec2(buttonPos.x + (buttonSize.x - ImGui::CalcTextSize(entry.path().filename().string().c_str()).x) / 2, originalPos.y + (buttonSize.y + padding.y)); // Recalculates the text position with the x position being reset.
                    }

                    if (entry.is_directory())
                    {
                        ImGui::PushID(counter);
                        ImGui::SetCursorPos(buttonPos);

                        if (ImGui::ImageButton((void*)folderIcon, buttonSize, ImVec2(0, 1), ImVec2(1, 0)))
                        {
                            currentPath = std::string(entry.path().string());
                        }

                        ImGui::SetCursorPos(textPos);

                        ImGui::Text(entry.path().filename().string().c_str());
                        ImGui::SameLine();
                        ImGui::PopID();

                        counter++;
                    }
                    else
                    {
                        if (strstr(entry.path().filename().string().c_str(), searchTerm.c_str()) != nullptr)
                        {
                            ImGui::PushID(counter);
                            ImGui::SetCursorPos(buttonPos);

                            unsigned int fileThumbnail = fileIcon; // Sets the thumbnail image to the generic file icon by default.
                            int fileNameLength = entry.path().filename().string().find_last_of('.'); // Gets the length of the file name up to the file extension.

                            std::string fileExtension = entry.path().filename().string().substr(fileNameLength); // Gets the file extension.

                            // Sets the correct file thumbnail based on the extension.
                            if (fileExtension == ".wav")
                            {
                                fileThumbnail = wavFileIcon;
                            }
                            else if (fileExtension == ".ttf" || fileExtension == ".otf")
                            {
                                fileThumbnail = fontFileIcon;
                            }
                            else if (fileExtension == ".worldOB")
                            {
                                fileThumbnail = sceneFileIcon;
                            }
                            else if (fileExtension == ".lua")
                            {
                                fileThumbnail = scriptIcon;
                            }
                            else if (fileExtension == ".png" || fileExtension == ".jpg" || fileExtension == ".gif" || fileExtension == ".PNG" || fileExtension == ".JPG")
                            {
                                fileThumbnail = imageFileIcon;
                            }

                            // Creates the file buttons.
                            if (ImGui::ImageButton((void*)fileThumbnail, buttonSize, ImVec2(0, 1), ImVec2(1, 0)))
                            {
                                if (ImGui::BeginDragDropSource())
                                {
                                    std::string path = entry.path().filename().string();
                                    ImGui::Text("Dragging");
                                    ImGui::SetDragDropPayload("ITEM_DRAG", &path, sizeof(path)); // Set payload
                                    ImGui::EndDragDropSource();
                                }

                                // Checks if the file is an engine scene file.
                                if (fileExtension == ".worldOB")
                                {
                                    std::size_t pos = entry.path().filename().string().find(fileExtension); // Gets the position of the file extension part of the path.
                                    currentScene = fileManager.LoadSceneFile(entry.path().filename().string().substr(0, pos), assetsPath + tempPath + "\\" + entry.path().filename().string());

                                    Core.renderer.objectsToRender = currentScene.objectsToRender;
                                    Core.renderer.RegenerateObjects();

                                    savedChanges = true;
                                    SearchMainCamera();
                                }
                            };

                            ImGui::SetCursorPos(textPos);

                            ImGui::Text(entry.path().filename().string().c_str()); // Creates the file label.
                            ImGui::SameLine();
                            ImGui::PopID();

                            counter++;
                        }
                    }
                }

                ImGui::EndChild();
            }

            ImGui::SetWindowFontScale(1.0f);
            ImGui::EndTable();
        }

        ImGui::EndChild();
        ImGui::End();
    }

    
}

void Editor::MenuBar()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New Scene"))
            {
                currentScene.sceneName = "Untitled";
                currentScene.objectsToRender.clear();
                Core.renderer.objectsToRender = currentScene.objectsToRender;

                Core.renderer.RegenerateObjects();
            }
            AddTooltip("Create a new, blank scene."); // Add tooltip for UI element above.

            if (ImGui::MenuItem("Open Scene"))
            {
                const char* filterTypes[1] = { "*.worldOB" }; // Defines the filters in the file explorer.
                std::string filePath = fileManager.OpenFileExplorer(filterTypes, "Open a scene", assetsPath); // Gets the full path of the selected file.

                if (!filePath.empty())
                {
                    std::string tempPath = filePath.erase(0, assetsPath.length());

                    int fileNamePos = tempPath.find_last_of('\\') + 1; // Gets the final directory position in the file path.
                    int fileExtensionPos = tempPath.find_last_of('.'); // Gets the extension position in the file path.

                    std::string fileName = tempPath.substr(fileNamePos, fileExtensionPos); // Gets the file name from the file path.

                    currentScene = fileManager.LoadSceneFile(fileName, tempPath);

                    Core.renderer.objectsToRender = currentScene.objectsToRender;
                    Core.renderer.RegenerateObjects();
                }
            }
            AddTooltip("Open a scene from your files window."); // Add tooltip for UI element above.

            /*if (ImGui::MenuItem("Open Recent"))
            {

            }*/

            if (ImGui::MenuItem("Save"))
            {
                Scene test;
                test.sceneName = currentScene.sceneName;
                test.scenePath = currentScene.scenePath;
                test.objectsToRender = Core.renderer.objectsToRender;

                fileManager.CreateSceneFile(test, test.sceneName, test.scenePath);
                savedChanges = true;
            }
            AddTooltip("Saves the current scene. Make sure to do this regularly!"); // Add tooltip for UI element above.

            if (ImGui::MenuItem("Save As"))
            {
                const char* filterTypes[1] = { "*.worldOB" }; // Defines the filters in the file explorer.
                const char* savePath = fileManager.SaveFileExplorer(filterTypes, "Save scene", assetsPath); // Gets the full path of the selected file.

                if (savePath) 
                {
                    const char* file_name = strrchr(savePath, '/');  // For Unix-based systems.
                    if (!file_name) 
                    {
                        file_name = strrchr(savePath, '\\');  // For Windows paths.
                    }

                    if (file_name) 
                    {
                        file_name++;  // Move past the slash to get the file name.
                    }
                    else 
                    {
                        file_name = savePath;  // If no slash was found, the entire string is the file name.
                    }

                    // Now strip the extension by finding the last dot ('.')
                    char file_name_no_ext[256];  // Buffer to store the file name without extension.
                    strcpy(file_name_no_ext, file_name);  // Copy file name.
                    char* dot = strrchr(file_name_no_ext, '.');

                    if (dot) 
                    {
                        *dot = '\0';  // Truncate at the dot to remove the extension.
                    }

                    Scene test;
                    test.sceneName = file_name_no_ext;
                    test.scenePath = savePath;
                    test.objectsToRender = Core.renderer.objectsToRender;

                    std::string tempPath = savePath;
                    std::string newFileName = tempPath.erase(0, assetsPath.length());

                    fileManager.CreateSceneFile(test, test.sceneName, assetsPath + newFileName);
                }
            }
            AddTooltip("Save the scene manually through your files window."); // Add tooltip for UI element above.

            if (ImGui::MenuItem("Exit Editor"))
            {
                applicationRunning = false;
            }
            AddTooltip("Quit the application."); // Add tooltip for UI element above.

            ImGui::EndMenu();
        }
        AddTooltip("Save or open a scene."); // Add tooltip for UI element above.

        if (ImGui::BeginMenu("Project"))
        {
            if (ImGui::MenuItem("Project Settings"))
            {
                selectedEditorConfig.showProjSettings = !selectedEditorConfig.showProjSettings;
            }

            ImGui::EndMenu();
        }
        AddTooltip("Tweak project settings."); // Add tooltip for UI element above.

        if (ImGui::BeginMenu("Editor"))
        {
            if (ImGui::MenuItem("Toggle FPS"))
            {
                selectedEditorConfig.showFPS = !selectedEditorConfig.showFPS;
            }

            if (ImGui::MenuItem("Toggle VSync"))
            {
                vsyncEnabled = (vsyncEnabled == 0) ? 1 : 0;
            }

            if (ImGui::MenuItem("Toggle Wireframe Mode"))
            {
                selectedEditorConfig.showWireframe = !selectedEditorConfig.showWireframe;

                if (selectedEditorConfig.showWireframe)
                {
                    GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
                }
                else
                {
                    GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
                }
            }

            if (ImGui::MenuItem("Advanced Settings"))
            {
                selectedEditorConfig.showTooltips = !selectedEditorConfig.showTooltips;
            }
            AddTooltip("You can turn off tooltips here."); // Add tooltip for UI element above.

            ImGui::EndMenu();
        }
        AddTooltip("Tweak editor settings."); // Add tooltip for UI element above.

        if (ImGui::BeginMenu("Window"))
        {
            if (ImGui::MenuItem("Rendering Stats Window"))
            {
                selectedEditorConfig.showRenderingStats = !selectedEditorConfig.showRenderingStats;
            }
            AddTooltip("Shows how many textures are created, how many are used, etc."); // Add tooltip for UI element above.

            if (ImGui::MenuItem("Toggle Console Window"))
            {
                Core.ToggleConsoleWindow();
                selectedEditorConfig.showConsoleWindow = Core.GetConsoleWindowState();
            }
            AddTooltip("Shows/hides the console window for debugging."); // Add tooltip for UI element above.

            ImGui::EndMenu();
        }
        AddTooltip("Toggle editor windows."); // Add tooltip for UI element above.

        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("About"))
            {

            }

            if (ImGui::MenuItem("Documentation"))
            {

            }

            ImGui::EndMenu();
        }
        AddTooltip("Opens the 'About' and 'Documentation' pages."); // Add tooltip for UI element above.

        ImGui::SameLine();

        std::string sceneTitle = (savedChanges) ? currentScene.sceneName : currentScene.sceneName + " - Unsaved Changes";

        ImVec2 sceneTitleSize = ImGui::CalcTextSize(sceneTitle.c_str());
        ImGui::SetCursorPosX((ImGui::GetContentRegionMax().x / 2.0f) - sceneTitleSize.x / 2);

        ImGui::Text(sceneTitle.c_str());
        AddTooltip("The scene name.");

        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetContentRegionMax().x / 1.2f);

        // Play scene button.
        if (ImGui::ImageButton((void*)playButton, ImVec2(ImGui::GetContentRegionMax().x / 120, ImGui::GetContentRegionAvail().y)))
        {
            savedChanges = false; // test unsaved indicator.

            Core.m_applicationState = OBApplicationState::Play;
        }
        AddTooltip("Play the scene."); // Add tooltip for UI element above.

        // Pause scene button.
        ImGui::SameLine();
        if (ImGui::ImageButton((void*)pauseButton, ImVec2(ImGui::GetContentRegionMax().x / 120, ImGui::GetContentRegionAvail().y)))
        {
            Core.m_applicationState = OBApplicationState::Pause;
        }
        AddTooltip("Pause the scene."); // Add tooltip for UI element above.

        // Stop scene button.
        ImGui::SameLine();
        if (ImGui::ImageButton((void*)stopButton, ImVec2(ImGui::GetContentRegionMax().x / 120, ImGui::GetContentRegionAvail().y)))
        {
            Core.m_applicationState = OBApplicationState::Stop;
        }
        AddTooltip("Stop the scene."); // Add tooltip for UI element above.

        ImGui::EndMainMenuBar();
    }
}

void Editor::AudioManagerComponent()
{
    // Right-click menu.
    if (ImGui::BeginPopupContextItem()) 
    {
        if (ImGui::MenuItem("Delete")) 
        {
            //renderer.objectsToRender[selectedObject]->RemoveComponent("Audio Manager");
        }
        ImGui::EndPopup();
    }

    ImGui::Text("Sounds: ");
    ImGui::SameLine();

    AudioManager audioManager = *Core.renderer.objectsToRender[selectedObject].GetComponent<AudioManager>();

    ImGui::Text(std::to_string(audioManager.sounds.size()).c_str()); // Shows the number of sounds in the vector.

    ImGui::SameLine();

    // New sound button.
    if (ImGui::Button("+"))
    {
        Sound s; // Creates a 'blank' sound.

        // Template sound details.
        s.soundName = "Enter sound name"; 
        s.filePath = "Enter path to .wav files";
        s.volume = 1;
        s.pitch = 1;
        s.position = glm::vec3(0);
        s.velocity = glm::vec3(0);

        audioManager.sounds.push_back(s); // Adds a sound to the list.
    }
    AddTooltip("Create a new sound effect."); // Add tooltip for UI element above.

    // Loops through every item in sounds vector.
    for (int i = 0; i < audioManager.sounds.size(); i++)
    {
        ImGui::Indent();

        ImGui::PushID(i);
        Sound sound = audioManager.sounds[i]; // Gets a sound from the vector.

        if (sound.soundName != audioManager.sounds[i].soundName) ImGui::SetNextItemOpen(true);

        // Collapsing header for selected sound.
        if (ImGui::CollapsingHeader(sound.soundName.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Indent();

            if (ImGui::BeginTable("SoundTable", 2))
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);

                ImGui::Text("Name:");

                ImGui::TableSetColumnIndex(1);

                ImGui::InputText("##name", &audioManager.sounds[i].soundName);

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);

                ImGui::Text("File:");

                ImGui::TableSetColumnIndex(1);

                ImGui::Text(audioManager.sounds[i].filePath.c_str());
                ImGui::SameLine();

                // Creates a button for the user to select a file through the file explorer.
                if (ImGui::ImageButton((void*)miniFolderIcon, ImVec2(20, 20), ImVec2(0, 1), ImVec2(1, 0)))
                {
                    audioManager.GenSound(i); // If a new sound has been created, generate sources and buffers for audio to play.
                    audioManager.Stop(audioManager.sounds[i].soundName); // Stops the audio source.

                    const char* filterTypes[1] = { "*.wav" }; // Defines the file filters.
                    std::string filePath = fileManager.OpenFileExplorer(filterTypes, "Select a sound file", assetsPath); // Gets the file path using the file explorer.

                    // Checks if file path is valid.
                    if (!filePath.empty())
                    {
                        audioManager.sounds[i].filePath = filePath.erase(0, assetsPath.length() + 1); // Updates the file path.

                        audioManager.sounds[i].audioSource->ChangeFile(audioManager.sounds[i].filePath); // Changes the file path for the audio source.
                        audioManager.sounds[i].audioSource->SetProperties(audioManager.sounds[i].pitch, audioManager.sounds[i].volume, audioManager.sounds[i].isLooping, audioManager.sounds[i].position, audioManager.sounds[i].velocity); // Resets audio source properties.
                    }
                }
                AddTooltip("Locate the file through your files window."); // Adds a tooltip to the UI element above.

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);

                ImGui::Text("Pitch:");

                ImGui::TableSetColumnIndex(1);

                ImGui::SliderFloat("##pitch", &audioManager.sounds[i].pitch, 0.1, 2.0, "%.2f");

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);

                ImGui::Text("Volume:");

                ImGui::TableSetColumnIndex(1);

                ImGui::SliderFloat("##volume", &audioManager.sounds[i].volume, 0.0, 1.0, "%.2f");

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);

                ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x / 4); // Set the width of each input field
                ImGui::Text("Position:");
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.3f, 0.3f, 1.0f)); // X color

                ImGui::TableSetColumnIndex(1);

                // The XYZ view.
                if (ImGui::BeginTable("TransformTable", 4))
                {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);

                    ImGui::Text("X");

                    ImGui::SameLine();
                    ImGui::SetCursorPosY(ImGui::GetCursorPos().y - 3);

                    ImGui::InputFloat("##PX", &audioManager.sounds[i].position.x, 0.0f, 0.0f, "%.f");
                    ImGui::PopStyleColor();

                    ImGui::TableSetColumnIndex(1);
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 1.0f, 0.3f, 1.0f)); // Y color

                    ImGui::SetCursorPosY(ImGui::GetCursorPos().y - 3);
                    ImGui::Text("Y");

                    ImGui::SameLine();
                    ImGui::InputFloat("##PY", &audioManager.sounds[i].position.y, 0.0f, 0.0f, "%.f");
                    ImGui::PopStyleColor();

                    ImGui::TableSetColumnIndex(2);

                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 0.3f, 1.0f, 1.0f)); // Z color

                    ImGui::SetCursorPosY(ImGui::GetCursorPos().y - 3);
                    ImGui::Text("Z");

                    ImGui::SameLine();
                    ImGui::InputFloat("##PZ", &audioManager.sounds[i].position.z, 0.0f, 0.0f, "%.f");
                    ImGui::PopStyleColor();

                    ImGui::EndTable();
                }

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);

                ImGui::Text("Velocity:");

                ImGui::TableSetColumnIndex(1);

                // The XYZ view.
                if (ImGui::BeginTable("TransformTable", 4))
                {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.3f, 0.3f, 1.0f)); // X color
                    ImGui::Text("X");

                    ImGui::SameLine();
                    ImGui::SetCursorPosY(ImGui::GetCursorPos().y - 3);

                    ImGui::InputFloat("##VX", &audioManager.sounds[i].velocity.x, 0.0f, 0.0f, "%.f");
                    ImGui::PopStyleColor();

                    ImGui::TableSetColumnIndex(1);

                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 1.0f, 0.3f, 1.0f)); // Y color

                    ImGui::SetCursorPosY(ImGui::GetCursorPos().y - 3);
                    ImGui::Text("Y");

                    ImGui::SameLine();
                    ImGui::InputFloat("##VY", &audioManager.sounds[i].velocity.y, 0.0f, 0.0f, "%.f");
                    ImGui::PopStyleColor();

                    ImGui::TableSetColumnIndex(2);

                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 0.3f, 1.0f, 1.0f)); // Z color

                    ImGui::SetCursorPosY(ImGui::GetCursorPos().y - 3);
                    ImGui::Text("Z");

                    ImGui::SameLine();
                    ImGui::InputFloat("##VZ", &audioManager.sounds[i].velocity.z, 0.0f, 0.0f, "%.f");
                    ImGui::PopStyleColor();

                    ImGui::EndTable();
                }

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);

                ImGui::Text("Play On Start-Up:");
                AddTooltip("Should the sound effect play when the scene starts?");

                ImGui::TableSetColumnIndex(1);

                ImGui::Checkbox("##playStart", &audioManager.sounds[i].playOnStartUp);

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);

                ImGui::Text("Repeat Delay:");
                AddTooltip("Should the sound effect not overlap another? This can be useful for voicelines.");

                ImGui::TableSetColumnIndex(1);

                ImGui::Checkbox("##repeat", &audioManager.sounds[i].repeatDelay);

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);

                ImGui::Text("Looped:");
                AddTooltip("Should the sound effect play constantly?");

                ImGui::TableSetColumnIndex(1);

                ImGui::Checkbox("##loop", &audioManager.sounds[i].isLooping);

                ImGui::NewLine();
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);

                ImGui::Text("Test:");

                ImGui::TableSetColumnIndex(1);

                // The play button.
                if (ImGui::Button("Play"))
                {
                    audioManager.GenSound(i); // If a new sound has been created, generate sources and buffers for audio to play.

                    audioManager.sounds[i].audioSource->Stop(); // Stops the sound from playing.

                    audioManager.sounds[i].audioSource->ChangeFile(audioManager.sounds[i].filePath); // Changes the file path for the audio source.
                    audioManager.sounds[i].audioSource->SetProperties(audioManager.sounds[i].pitch, audioManager.sounds[i].volume, audioManager.sounds[i].isLooping, audioManager.sounds[i].position, audioManager.sounds[i].velocity); // Resets audio source properties.
                    
                    audioManager.sounds[i].audioSource->Play(); // Plays the sound.
                }

                ImGui::SameLine();

                // The stop button.
                if (ImGui::Button("Pause"))
                {
                    audioManager.sounds[i].audioSource->Pause(); // Stops the sound.
                }

                ImGui::SameLine();

                // The stop button.
                if (ImGui::Button("Stop"))
                {
                    audioManager.sounds[i].audioSource->Stop(); // Stops the sound.
                }

                ImGui::EndTable();
            }

            ImGui::Unindent();
        }

        ImGui::PopID();
        ImGui::Unindent();
    }
}

void Editor::ScriptManagerComponent()
{
    ImGui::Text("Scripts: ");
    ImGui::SameLine();

    ScriptManager scriptManager = *Core.renderer.objectsToRender[selectedObject].GetComponent<ScriptManager>();

    ImGui::Text(std::to_string(scriptManager.GetScripts().size()).c_str()); // Shows the number of sounds in the vector.

    ImGui::SameLine();

    // New script button.
    if (ImGui::Button("Bind"))
    {
        const char* filterTypes[4] = { "*.lua" };
        const char* file_path = tinyfd_openFileDialog(
            "Open a lua script",              // Title of the dialog
            (assetsPath + "/Assets").c_str(),                         // Default path ("" means current directory)
            1,                          // Number of file filters
            filterTypes,                // File filters (e.g., ["*.txt"])
            "Lua script",                       // Filter description
            0                           // Allow multiple selections (0 for no)
        );

        // Checks if the file exists.
        if (file_path)
        {
            // Script changing logic here.
            scriptManager.BindScript(Core.m_scriptController.m_lua, file_path);
        }
    }
    AddTooltip("Bind an existing script."); // Add tooltip for UI element above.

    ImGui::SameLine();

    // New script button.
    if (ImGui::Button("Create"))
    {
        const char* filterTypes[1] = { "*.lua" }; // Defines the filters in the file explorer.
        const char* savePath = fileManager.SaveFileExplorer(filterTypes, "Save script", assetsPath); // Gets the full path of the selected file.

        if (savePath)
        {
            const char* file_name = strrchr(savePath, '/');  // For Unix-based systems.
            if (!file_name)
            {
                file_name = strrchr(savePath, '\\');  // For Windows paths.
            }

            if (file_name)
            {
                file_name++;  // Move past the slash to get the file name.
            }
            else
            {
                file_name = savePath;  // If no slash was found, the entire string is the file name.
            }

            // Now strip the extension by finding the last dot ('.')
            char file_name_no_ext[256];  // Buffer to store the file name without extension.
            strcpy(file_name_no_ext, file_name);  // Copy file name.
            char* dot = strrchr(file_name_no_ext, '.');

            if (dot)
            {
                *dot = '\0';  // Truncate at the dot to remove the extension.
            }

            try
            {
                std::filesystem::copy_file(defaultScriptPath, savePath); // Copies the template lua code from editor resources and renames the file.
                scriptManager.BindScript(Core.m_scriptController.m_lua, savePath);
            }
            catch (std::filesystem::filesystem_error& e)
            {
                std::cout << e.what();
            }
        }
    }
    AddTooltip("Create a new script."); // Add tooltip for UI element above.

    // Loops through every item in scripts vector.
    for (int i = 0; i < scriptManager.GetScripts().size(); i++)
    {
        ImGui::Indent();

        ImGui::PushID(i);
        Script s = scriptManager.GetScripts()[i]; // Gets a sound from the vector.

        if (s.GetPath() != scriptManager.GetScripts()[i].GetPath()) ImGui::SetNextItemOpen(true);

        std::filesystem::path scriptPath = s.GetPath();
        std::string scriptFolderPath = scriptPath.parent_path().string();
        std::string scriptName = s.GetPath().erase(0, scriptFolderPath.length() + 1); // Get the name of the script (e.g. 'test.lua')

        // Collapsing header for selected sound.
        if (ImGui::CollapsingHeader(scriptName.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Indent();

            if (ImGui::BeginTable("ScriptTable", 2))
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);

                ImGui::Text("Name:");

                ImGui::TableSetColumnIndex(1);

                ImGui::Text(scriptName.c_str());

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);

                ImGui::Text("File:");

                ImGui::TableSetColumnIndex(1);

                std::string scriptFilePath = scriptManager.GetScripts()[i].GetPath().erase(0, Core.selectedProject.assetsFolderPath.length() + 1);

                ImGui::Text(scriptFilePath.c_str());
                ImGui::SameLine();

                // Creates a button that allows the user to pick a script file. 
                if (ImGui::ImageButton((void*)miniFolderIcon, ImVec2(20, 20), ImVec2(0, 1), ImVec2(1, 0)))
                {
                    const char* filterTypes[4] = { "*.lua" };
                    const char* file_path = tinyfd_openFileDialog(
                        "Open a lua script",              // Title of the dialog
                        (assetsPath + "/Assets").c_str(),                         // Default path ("" means current directory)
                        1,                          // Number of file filters
                        filterTypes,                // File filters (e.g., ["*.txt"])
                        "Lua script",                       // Filter description
                        0                           // Allow multiple selections (0 for no)
                    );

                    // Checks if the file exists.
                    if (file_path)
                    {
                        // Script changing logic here.
                        scriptManager.GetScripts()[i].SetPath(file_path);
                    }
                }
                AddTooltip("Locate the file through your files window."); // Adds a tooltip to the UI element above.

                ImGui::SameLine();

                // New script button.
                if (ImGui::Button("-"))
                {
                    scriptManager.UnbindScript(scriptManager.GetScripts()[i].GetPath());
                }
                AddTooltip("Unbind the script."); // Add tooltip for UI element above.

                ImGui::EndTable();
            }

            ImGui::Unindent();
        }

        ImGui::PopID();
        ImGui::Unindent();
    }
}

void Editor::SearchMainCamera()
{
    for (auto& obj : Core.renderer.objectsToRender)
    {
        // TODO: make the main camera a separate thing and use that instead of the editor camera.
        /*editorCamera = *obj.GetComponent<Camera>();
        editorCameraObj = obj;*/
    }
}

void Editor::DebugWindow()
{
    ImGui::Begin("Debug Log");
    AddTooltip("This window is used for showing the log console."); // Add tooltip for UI element above.

    if (ImGui::Button("Clear")) DebugOB.ClearLog();
    AddTooltip("Clear all messages in the log console."); // Add tooltip for UI element above.

    ImGui::SameLine();
    if (ImGui::Button("Group"))
    {

    }
    AddTooltip("Group the same messages together.");

    if (ImGui::BeginChild("ConsoleOutput", ImVec2(0, ImGui::GetContentRegionAvail().y / 2.5), true, ImGuiWindowFlags_AlwaysVerticalScrollbar))
    {
        ImGui::PushFont(pixelFont);
        ImGui::TextUnformatted(Debug::Get().GetLogOutput().str().c_str());
        ImGui::PopFont();

        ImGui::EndChild();
    }
    AddTooltip("Use 'DebugOB.Log(x)' to make use of the console."); // Add tooltip for UI element above.

    ImGui::End();
}

void Editor::OptionalWindows()
{
    // Checks if the stats window should be visible.
    if (selectedEditorConfig.showRenderingStats)
    {
        ImGui::Begin("Rendering Stats"); // Sets the window title.

        ImGui::Text("Textures Loaded: %.0f", double(Core.renderer.texturesLoaded));
        ImGui::Text("New Textures Created: %.0f", double(Core.renderer.newTextures));
        ImGui::Text("Cached textures: %.0f", double(Core.renderer.GetCachedTextureCount()));

        ImGui::End();
    }

    // Checks if the project settings window should be visible.
    if (selectedEditorConfig.showProjSettings)
    {
        ImGui::Begin("Project Settings"); // Sets the window title.

        /*if (ImGui::BeginCombo("Select First Scene", items[current_item])) 
        {
            for (int n = 0; n < IM_ARRAYSIZE(items); n++) 
            {
                bool is_selected = (current_item == n);

                if (ImGui::Selectable(items[n], is_selected))
                    current_item = n;
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }*/

        // Displays the input text and its label.
        ImGui::Text("Enter Action Name:");
        ImGui::SameLine();
        ImGui::InputText("##actionInput", &inputString);

        // 'Add new action' button.
        if (ImGui::Button("Add"))
        {
            Project.InputManager.AddAction(Action(inputString));
        }

        ImGui::SameLine();

        if (ImGui::Button("Listen to Input")) Project.InputManager.listenToInput = !Project.InputManager.listenToInput;

        // Checks if the input manager is listening for keyboard input.
        if (Project.InputManager.listenToInput)
        {
            ImGui::SameLine();
            ImGui::Text("Listening..."); // Display feedback text.
        }

        // Shows the list of actions.
        if (ImGui::BeginListBox("Actions"))
        {
            // Loops through each input action.
            for (int i = 0; i < Project.InputManager.actionList.size(); i++)
            {
                ImGui::PushID(i);

                // Checks if the action header has been opened.
                if (ImGui::CollapsingHeader(("Name: " + Project.InputManager.actionList[i].GetActionName()).c_str()))
                {
                    Project.InputManager.selectedAction = i; // Updates the currently selected action.

                    // Loops through each keybind in the action.
                    for (int j = 0; j < Project.InputManager.actionList[i].GetKeyBinds().size(); j++)
                    {
                        if (ImGui::Selectable(Project.InputManager.actionList[i].GetKeyName(j)), keyBindIndex == j)
                        {

                        }

                        ImGui::SameLine();

                        // 'Delete keybind' button.
                        if (ImGui::Button("Delete##1"))
                        {
                            Project.InputManager.actionList[i].DeleteKeyBind(j); // Deletes the selected keybind.
                        }
                    }
                }

                // Moves the delete button position.
                ImGui::SetCursorPos(ImVec2(ImGui::GetContentRegionMax().x + 100, ImGui::GetCursorPos().y));
                ImGui::SameLine();

                // 'Delete action' button.
                if (ImGui::Button("Delete##2"))
                {
                    Project.InputManager.DeleteAction(i);
                }

                ImGui::PopID(); // Pops the style to prevent changes to other UI components.
            }

            ImGui::EndListBox();
        }

        ImGui::End();
    }
}

void Editor::AddTooltip(const char* message) const
{
    if (!selectedEditorConfig.showTooltips) return; // Don't show a tooltip if it's been disabled.
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) ImGui::SetTooltip(message); // Add tooltip for UI element.
}

void Editor::CreateTransformColumn(const std::array<std::string, 3>& colNames, std::array<Vector3, 3> values)
{
    for (int i = 0; i < colNames.size(); i++)
    {

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);

        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x / 4); // Set the width of each input field
        ImGui::Text((colNames[i]).c_str());

        char colLetter = *(colNames[i]).c_str();

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.3f, 0.3f, 1.0f)); // X color

        ImGui::TableSetColumnIndex(1);

        ImGui::Text("X");

        ImGui::SameLine();
        ImGui::SetCursorPosY(ImGui::GetCursorPos().y - 3);
        ImGui::PushID(i);

        ImGui::InputFloat("##" + colLetter + 'X', &values[i].x, 0.0f, 0.0f, " % .f");
        ImGui::PopStyleColor();

        ImGui::TableSetColumnIndex(2);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 1.0f, 0.3f, 1.0f)); // Y color

        ImGui::SetCursorPosY(ImGui::GetCursorPos().y - 3);
        ImGui::Text("Y");

        ImGui::SameLine();
        ImGui::InputFloat("##" + colLetter + 'Y', &values[i].y, 0.0f, 0.0f, "%.f");
        ImGui::PopStyleColor();

        ImGui::TableSetColumnIndex(3);

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 0.3f, 1.0f, 1.0f)); // Z color

        ImGui::SetCursorPosY(ImGui::GetCursorPos().y - 3);
        ImGui::Text("Z");

        ImGui::SameLine();
        ImGui::InputFloat("##" + colLetter + 'Z', &values[i].z, 0.0f, 0.0f, "%.f");
        ImGui::PopStyleColor();

        ImGui::PopID();
    }
}
