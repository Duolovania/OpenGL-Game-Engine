#include "editor.h"
#include "tinyfiledialogs/tinyfiledialogs.h"
#include <cmath>
#include <filesystem>

FileManager fileManager;
int selectedObject = -2;
std::string inputString, searchTerm, rootPath, currentPath;

float sprintSpeed;
float iconSize = 200;

bool showProjSettings = false;

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

std::unique_ptr<Texture> iconTextures;
GLuint64 playButton, pauseButton, stopButton, fileIcon, folderIcon, wavFileIcon, fontFileIcon, sceneFileIcon, imageFileIcon, miniFolderIcon, resetIcon;

void Editor::Init(GLFWwindow* window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    StylesConfig();

    fbShader = std::make_unique<Shader>("../OrbiterCore/Res/Shaders/Framebuffer.shader");
    fbShader->CreateShader();
    fbShader->Bind();

    renderer.Init();

    fbShader->Bind();
    fbShader->SetUniform1i("screenTexture", 1);

    frameBufferVA = std::make_unique<VertexArray>();
    frameBufferVA->Gen();
    frameBufferVA->Bind();

    frameBufferVB = std::make_unique<VertexBuffer>(sizeof(rectangleVertices));
    frameBufferVB->Gen(&rectangleVertices);

    VertexBufferLayout layout;
    layout.Push<float>(2);
    layout.Push<float>(2);

    frameBufferVA->AddBuffer(*frameBufferVB, layout);

    currentPath = std::filesystem::current_path().string() + "\\Assets";
    rootPath = currentPath;

    iconTextures = std::make_unique<Texture>("../OrbiterCore/Res/Application Icons/playbutton.png");

    playButton = iconTextures->Load("../OrbiterCore/Res/Application Icons/playbutton.png", true);
    pauseButton = iconTextures->Load("../OrbiterCore/Res/Application Icons/pausebutton.png", true);
    stopButton = iconTextures->Load("../OrbiterCore/Res/Application Icons/stopbutton.png", true);

    folderIcon = iconTextures->Load("../OrbiterCore/Res/Application Icons/foldericon.png", true);
    fileIcon = iconTextures->Load("../OrbiterCore/Res/Application Icons/fileicon.png", true);

    wavFileIcon = iconTextures->Load("../OrbiterCore/Res/Application Icons/wavfileicon.png", true);
    fontFileIcon = iconTextures->Load("../OrbiterCore/Res/Application Icons/fontfileicon.png", true);
    sceneFileIcon = iconTextures->Load("../OrbiterCore/Res/Application Icons/scenefileicon.png", true);
    imageFileIcon = iconTextures->Load("../OrbiterCore/Res/Application Icons/imagefileicon.png", true);
    miniFolderIcon = iconTextures->Load("../OrbiterCore/Res/Application Icons/foldericon - mini.png", true);
    resetIcon = iconTextures->Load("../OrbiterCore/Res/Application Icons/reset.png", true);

    currentScene.sceneName = "Untitled";
    currentScene.objectsToRender.clear();
    renderer.objectsToRender = currentScene.objectsToRender;

    renderer.RegenerateObjects();
}

bool Editor::OnUpdate(float deltaTime, float time)
{
    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();

    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_None);

    MenuBar();

    ContentBrowser();
    Inspector();

    Hierarchy();
    Viewport();

    if (viewportSize.x < 0 || viewportSize.y < 0)
    {
        ImGui::Render();
        return true;
    }

    framebuffer->Bind();

    // Check if the camera does not exist.
    if (camera != nullptr)
    {
        GLCall(glClearColor(camera->backgroundColor[0], camera->backgroundColor[1], camera->backgroundColor[2], 1.0f));
        GLCall(glClear(GL_COLOR_BUFFER_BIT));

        sprintSpeed = Core.InputManager.GetActionStrength("sprint") * 150; // Camera movement sprint speed.

        cameraObj->transform.scale = Vector3(viewportSize.x, viewportSize.y, 0);
        cameraObj->transform.position += Vector2(Core.InputManager.BasicMovement().x * (100.0f + sprintSpeed) * deltaTime, Core.InputManager.BasicMovement().y * (100.0f + sprintSpeed) * deltaTime);

        // Logic for play button test.
        if (isPlaying)
        {
            renderer.objectsToRender[0]->transform.position.y = (sin((time) * 1)) * 100; // sine wave movement.
        }

        // Render scene objects.
        renderer.Draw(glm::ortho(((float)viewportSize.x / (float)viewportSize.y) * -100, ((float)viewportSize.x / (float)viewportSize.y) * 100, -100.0f, 100.0f, -1.0f, 1.0f), cameraObj->GetView(), { camera->outputColor[0], camera->outputColor[1], camera->outputColor[2], camera->outputColor[3] });
    }
    else
    {
        GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f)); // Output black screen.
        GLCall(glClear(GL_COLOR_BUFFER_BIT));
    }
    

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

    if (showStats)
    {
        ImGui::Begin("Rendering Stats");

        ImGui::Text("Textures Loaded: %.0f", double(renderer.texturesLoaded));
        ImGui::Text("New Textures Created: %.0f", double(renderer.newTextures));

        ImGui::End();
    }

    if (showProjSettings)
    {
        ImGui::Begin("Project Settings");

        ImGui::Text("Enter Action Name:");
        ImGui::SameLine();

        ImGui::InputText("##actionInput", &inputString);

        if (ImGui::Button("Add"))
        {
            Core.InputManager.AddAction(Action(inputString));
        }

        ImGui::SameLine();

        if (ImGui::Button("Listen to Input")) Core.InputManager.listenToInput = !Core.InputManager.listenToInput;

        if (Core.InputManager.listenToInput)
        {
            ImGui::SameLine();
            ImGui::Text("Listening...");
        }

        if (ImGui::BeginListBox("Actions"))
        {
            for (int i = 0; i < Core.InputManager.actionList.size(); i++)
            {
                ImGui::PushID(i);
                if (ImGui::CollapsingHeader(("Name: " + Core.InputManager.actionList[i].GetActionName()).c_str()))
                {
                    Core.InputManager.selectedAction = i;

                    for (int j = 0; j < Core.InputManager.actionList[i].GetKeyBinds().size(); j++)
                    {
                        if (ImGui::Selectable(Core.InputManager.actionList[i].GetKeyName(j)), keyBindIndex == j)
                        {

                        }

                        ImGui::SameLine();

                        if (ImGui::Button("Delete##1"))
                        {
                            Core.InputManager.actionList[i].DeleteKeyBind(j);
                        }
                    }
                }

                ImGui::SetCursorPos(ImVec2(ImGui::GetContentRegionMax().x + 100, ImGui::GetCursorPos().y));

                ImGui::SameLine();

                if (ImGui::Button("Delete##2"))
                {
                    Core.InputManager.DeleteAction(i);
                }

                ImGui::PopID();
            }

            ImGui::EndListBox();
        }

        ImGui::End();
    }

    framebuffer->UnBind();
    fbShader->Bind();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    return applicationRunning;
}

void Editor::Close()
{
    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
}

void Editor::CleanUp()
{
    
}

void Editor::StylesConfig()
{
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.FontDefault = io.Fonts->AddFontFromFileTTF("../OrbiterCore/Res/Fonts/scada/Scada-Regular.ttf", 18.0f);

    io.FontDefault = io.Fonts->AddFontFromFileTTF("../OrbiterCore/Res/Fonts/open-sans/OpenSans-Semibold.ttf", 18.0f);
    
    pixelFont = io.Fonts->AddFontFromFileTTF("../OrbiterCore/Res/Fonts/joystix/joystix monospace.otf", 10.0f);

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
    if (framebuffer->GetSize() != glm::vec2(viewportSize.x, viewportSize.y)) framebuffer->Resize(glm::vec2(viewportSize.x, viewportSize.y));

    ImGui::Image((void*)framebuffer->GetTexture(), ImVec2(framebuffer->GetSize().x, framebuffer->GetSize().y), ImVec2(0, 1), ImVec2(1, 0));
    ImVec2 imagePos = ImGui::GetCursorScreenPos();

    ImVec2 position = ImVec2(imagePos.x * 1.15f, imagePos.y / 10.15f);
    ImGui::SetCursorScreenPos(position);

    if (showFPS)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.988f, 0.659f, 0.176f, 1.0f));
        ImGui::Text("FPS: %.1f", double(1.0f / ImGui::GetIO().DeltaTime));
        ImGui::PopStyleColor();
    }

    ImGui::End();
    ImGui::PopStyleVar();
}

// Function to recursively display hierarchy
void ShowHierarchy(const std::string& name, int index, bool isLeaf = false) 
{
    // Check if the node is a leaf node or can be expanded
    if (isLeaf) 
    {
        if (ImGui::Selectable(name.c_str(), false)) 
        {
            selectedObject = index;

        }
    }
    else 
    {
        if (ImGui::TreeNode(name.c_str())) 
        {
            selectedObject = index;

            // Example of child nodes (these can be dynamic)
            ShowHierarchy("Child 1", 0, true);

            ImGui::TreePop();
        }
    }
}

void Editor::Hierarchy()
{
    ImGui::Begin("Hierarchy");
    AddTooltip("This window shows a list of all objects in the scene."); // Add tooltip for UI element above.

    if (ImGui::Button("Add"))
    {
        std::shared_ptr<GameObject> newGObj = std::make_unique<GameObject>();
        newGObj->transform.position = Vector3();

        renderer.objectsToRender.push_back(newGObj);
        renderer.RegenerateObject(renderer.objectsToRender.size() - 1);
    }
    AddTooltip("Adds a blank GameObject to the scene."); // Add tooltip for UI element above.

    for (int i = 0; i < renderer.objectsToRender.size(); i++)
    {
        ImGui::PushID(i);
        ShowHierarchy(renderer.objectsToRender[i]->objectName, i, true);
        ImGui::PopID();
    }

    ImGui::End();
}

void Editor::Inspector()
{
    ImGui::Begin("Inspector");
    AddTooltip("This window covers the details for each component in the GameObject."); // Add tooltip for UI element above.

    if (selectedObject >= 0 && renderer.objectsToRender.size() >= 1)
    {
        ImGui::InputText("##label0", &renderer.objectsToRender[selectedObject]->objectName);

        if (cameraObj != nullptr)
        {
            ImGui::SameLine();

            if (ImGui::Button("Jump To"))
            {
                cameraObj->transform.position = renderer.objectsToRender[selectedObject]->transform.position;
            }
            AddTooltip("See where this object is located on the scene."); // Add tooltip for UI element above.
        }

        // Check that there is more than 1 object in the scene.
        if (renderer.objectsToRender.size() > 1) 
        {
            ImGui::SameLine();

            if (ImGui::Button("Delete"))
            {
                renderer.objectsToRender.erase(renderer.objectsToRender.begin() + selectedObject);
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

                ImGui::InputFloat("##PX", &renderer.objectsToRender[selectedObject]->transform.position.x, 0.0f, 0.0f, "%.f");
                ImGui::PopStyleColor();

                ImGui::TableSetColumnIndex(2);
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 1.0f, 0.3f, 1.0f)); // Y color

                ImGui::SetCursorPosY(ImGui::GetCursorPos().y - 3);
                ImGui::Text("Y");

                ImGui::SameLine();
                ImGui::InputFloat("##PY", &renderer.objectsToRender[selectedObject]->transform.position.y, 0.0f, 0.0f, "%.f");
                ImGui::PopStyleColor();

                ImGui::TableSetColumnIndex(3);

                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 0.3f, 1.0f, 1.0f)); // Z color

                ImGui::SetCursorPosY(ImGui::GetCursorPos().y - 3);
                ImGui::Text("Z");

                ImGui::SameLine();
                ImGui::InputFloat("##PZ", &renderer.objectsToRender[selectedObject]->transform.position.z, 0.0f, 0.0f, "%.f");
                ImGui::PopStyleColor();


                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);

                ImGui::Text("Rotation:");

                ImGui::TableSetColumnIndex(1);
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.3f, 0.3f, 1.0f)); // X color

                ImGui::Text("X");

                ImGui::SameLine();
                ImGui::SetCursorPosY(ImGui::GetCursorPos().y - 3);

                ImGui::InputFloat("##RX", &renderer.objectsToRender[selectedObject]->transform.rotation.x, 0.0f, 0.0f, "%.f");
                ImGui::PopStyleColor();

                ImGui::TableSetColumnIndex(2);

                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 1.0f, 0.3f, 1.0f)); // Y color

                ImGui::SetCursorPosY(ImGui::GetCursorPos().y - 3);
                ImGui::Text("Y");

                ImGui::SameLine();
                ImGui::InputFloat("##RY", &renderer.objectsToRender[selectedObject]->transform.rotation.y, 0.0f, 0.0f, "%.f");
                ImGui::PopStyleColor();

                ImGui::TableSetColumnIndex(3);

                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 0.3f, 1.0f, 1.0f)); // Z color

                ImGui::SetCursorPosY(ImGui::GetCursorPos().y - 3);
                ImGui::Text("Z");

                ImGui::SameLine();
                ImGui::InputFloat("##RZ", &renderer.objectsToRender[selectedObject]->transform.rotation.z, 0.0f, 0.0f, "%.f");
                ImGui::PopStyleColor();

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);

                ImGui::Text("Scale:");
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.3f, 0.3f, 1.0f)); // X color

                ImGui::TableSetColumnIndex(1);

                ImGui::Text("X");

                ImGui::SameLine();
                ImGui::SetCursorPosY(ImGui::GetCursorPos().y - 3);

                ImGui::InputFloat("##SX", &renderer.objectsToRender[selectedObject]->transform.scale.x, 0.0f, 0.0f, "%.f");
                ImGui::PopStyleColor();

                ImGui::TableSetColumnIndex(2);

                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 1.0f, 0.3f, 1.0f)); // Y color

                ImGui::SetCursorPosY(ImGui::GetCursorPos().y - 3);
                ImGui::Text("Y");

                ImGui::SameLine();
                ImGui::InputFloat("##SY", &renderer.objectsToRender[selectedObject]->transform.scale.y, 0.0f, 0.0f, "%.f");
                ImGui::PopStyleColor();

                ImGui::TableSetColumnIndex(3);

                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 0.3f, 1.0f, 1.0f)); // Z color

                ImGui::SetCursorPosY(ImGui::GetCursorPos().y - 3);
                ImGui::Text("Z");

                ImGui::SameLine();
                ImGui::InputFloat("##SZ", &renderer.objectsToRender[selectedObject]->transform.scale.z, 0.0f, 0.0f, "%.f");
                ImGui::PopStyleColor();

                ImGui::PopItemWidth(); // Reset item width

                /*std::array<Vector3, 3> thigny = { renderer.objectsToRender[selectedObject].transform.position, renderer.objectsToRender[selectedObject].transform.rotation, renderer.objectsToRender[selectedObject].transform.scale };
                CreateTransformColumn({ "Position", "Rotation", "Scale" }, thigny);*/

                ImGui::EndTable();
            }
        }

        // Checks if the object has a sprite renderer component before showing the dropdown.
        if (renderer.objectsToRender[selectedObject]->HasComponent("Sprite Renderer"))
        {
            if (ImGui::CollapsingHeader("Sprite Renderer"))
            { 
                std::shared_ptr<SpriteRenderer> spriteRenderer = renderer.objectsToRender[selectedObject]->GetComponent<SpriteRenderer>();

                ImGui::Image((void*)spriteRenderer->cTexture.textureBuffer, ImVec2(200, 200), ImVec2(0, 1), ImVec2(1, 0));

                ImGui::SameLine();

                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 1.0f, 0.3f, 0.5f)); // Sets the text to green.

                std::string imageName = spriteRenderer->cTexture.m_imagePath.erase(0, spriteRenderer->cTexture.m_imagePath.find_last_of("/") + 1); // Cuts the image directory, leaving just the name and extension.
                ImGui::Text(imageName.c_str());

                ImGui::PopStyleColor(); // Pops the style.

                ImGui::SameLine();

                // Creates a button that allows the user to pick an image file. 
                if (ImGui::ImageButton((void*)miniFolderIcon, ImVec2(20, 20), ImVec2(0, 1), ImVec2(1, 0)))
                {
                    const char* filterTypes[4] = { "*.png", "*.jpg"};
                    const char* file_path = tinyfd_openFileDialog(
                        "Open an image file",              // Title of the dialog
                        (rootPath + "/Assets").c_str(),                         // Default path ("" means current directory)
                        2,                          // Number of file filters
                        filterTypes,                // File filters (e.g., ["*.txt"])
                        "Image",                       // Filter description
                        0                           // Allow multiple selections (0 for no)
                    );

                    // Checks if the file exists.
                    if (file_path)
                    {
                        spriteRenderer->cTexture.m_imagePath = "Assets" + std::string(file_path).erase(0, rootPath.length()); // Erases the directories leading up to the "Assets" folder.
                        renderer.RegenerateObject(selectedObject); // Updates the image to apply changes.
                    }
                }
                AddTooltip("Locate the file through your files window."); // Adds a tooltip to the UI element above.

                ImGui::Text("Colour:");
                ImGui::SameLine();
                ImGui::ColorEdit4("##label4", (float*)&spriteRenderer->color);

                ImGui::SameLine();

                if (ImGui::ImageButton((void*) resetIcon, ImVec2(20, 20), ImVec2(0, 1), ImVec2(1, 0)))
                {
                    spriteRenderer->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
                }
                AddTooltip("Reset to default."); // Adds a tooltip to the UI element above.
            }
        }
        else
        {
            // Check if the object does not have a camera component before providing the option to add a sprite renderer.
            if (selectedObject >= 0 && !renderer.objectsToRender[selectedObject]->HasComponent("Camera"))
            {
                if (ImGui::Button("Add Sprite Renderer"))
                {
                    SpriteRenderer spriteRendererComponent;
                    renderer.objectsToRender[selectedObject]->AddComponent(spriteRendererComponent);
                }
            }
        }

        // Checks if the object has a camera component before showing the dropdown.
        if (renderer.objectsToRender[selectedObject]->HasComponent("Camera"))
        {
            cameraObj = renderer.objectsToRender[selectedObject]; // Sets this gameobject as the scene camera.

            if (ImGui::CollapsingHeader("Camera Output"))
            {
                camera = renderer.objectsToRender[selectedObject]->GetComponent<Camera>();

                ImGui::Image((void*)framebuffer->GetTexture(), ImVec2(200, 200), ImVec2(0, 1), ImVec2(1, 0));

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
            }
        }
        else
        {
            // Check if the object does not have a sprite renderer component before providing the option to add a camera.
            if (selectedObject >= 0 && !renderer.objectsToRender[selectedObject]->HasComponent("Sprite Renderer"))
            {
                if (ImGui::Button("Add Camera"))
                {
                    Camera cameraComponent;
                    renderer.objectsToRender[selectedObject]->AddComponent(cameraComponent);
                }
            }
        }

        // Checks if the object has an audio manager component before showing the dropdown.
        if (renderer.objectsToRender[selectedObject]->HasComponent("Audio Manager"))
        {
            if (ImGui::CollapsingHeader("AudioManager"))
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
                    renderer.objectsToRender[selectedObject]->AddComponent(audioManagerComponent);
                }
            }
        }
    }

    ImGui::End();
}

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

void Editor::ContentBrowser()
{
    ImGui::Begin("Assets Folder");
    std::string toolTipMsg = "This is a repository for all your game assets. This can be found at: " + rootPath;
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
            ShowFolders(rootPath);

            ImGui::EndChild();
        }

        ImGui::TableSetColumnIndex(1);

        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + 10, ImGui::GetCursorPos().y));

        if (ImGui::Button("<"))
        {
            std::filesystem::path parentDir = currentPath;
            currentPath = parentDir.parent_path().string();

            if (currentPath.length() < rootPath.length()) currentPath = rootPath;
        }

        ImGui::SameLine();

        ImGui::Text("Search");
        AddTooltip("Search for an item in the folder."); // Add tooltip for UI element above.
        
        ImGui::SameLine();
        ImGui::InputText("##search", &searchTerm);

        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + 10, ImGui::GetCursorPos().y));

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.224, 0.482, 0.267, 1.0f));

        std::string tempPath = currentPath;
        std::string selectedPath = tempPath.erase(0, rootPath.length());

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
                                currentScene = fileManager.LoadYAMLFile(entry.path().filename().string().substr(0, pos), tempPath + "\\" + entry.path().filename().string());

                                renderer.objectsToRender = currentScene.objectsToRender;
                                renderer.RegenerateObjects();

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
                renderer.objectsToRender = currentScene.objectsToRender;

                renderer.RegenerateObjects();
            }
            AddTooltip("Create a new, blank scene."); // Add tooltip for UI element above.

            if (ImGui::MenuItem("Open Scene"))
            {
                const char* filterTypes[1] = { "*.worldOB" }; // Defines the filters in the file explorer.
                std::string filePath = fileManager.OpenFileExplorer(filterTypes, "Open a scene", rootPath); // Gets the full path of the selected file.

                if (!filePath.empty())
                {
                    std::string tempPath = filePath.erase(0, rootPath.length());

                    int fileNamePos = tempPath.find_last_of('\\') + 1; // Gets the final directory position in the file path.
                    int fileExtensionPos = tempPath.find_last_of('.'); // Gets the extension position in the file path.

                    std::string fileName = tempPath.substr(fileNamePos, fileExtensionPos); // Gets the file name from the file path.

                    currentScene = fileManager.LoadYAMLFile(fileName, tempPath);

                    renderer.objectsToRender = currentScene.objectsToRender;
                    renderer.RegenerateObjects();
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
                test.objectsToRender = renderer.objectsToRender;

                fileManager.CreateYAMLFile(test, test.sceneName, test.scenePath);
                savedChanges = true;
            }
            AddTooltip("Saves the current scene. Make sure to do this regularly!"); // Add tooltip for UI element above.

            if (ImGui::MenuItem("Save As"))
            {
                const char* filterTypes[1] = { "*.worldOB" }; // Defines the filters in the file explorer.
                const char* savePath = fileManager.SaveFileExplorer(filterTypes, "Save scene", rootPath); // Gets the full path of the selected file.

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
                    test.objectsToRender = renderer.objectsToRender;

                    std::string tempPath = savePath;
                    std::string newFileName = tempPath.erase(0, rootPath.length());

                    fileManager.CreateYAMLFile(test, test.sceneName, "Assets" + newFileName);
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
                showProjSettings = !showProjSettings;
            }

            ImGui::EndMenu();
        }
        AddTooltip("Tweak project settings."); // Add tooltip for UI element above.

        if (ImGui::BeginMenu("Editor"))
        {
            if (ImGui::MenuItem("Toggle FPS"))
            {
                showFPS = !showFPS;
            }

            if (ImGui::MenuItem("Toggle VSync"))
            {
                vsyncEnabled = (vsyncEnabled == 0) ? 1 : 0;
            }

            if (ImGui::MenuItem("Toggle Wireframe Mode"))
            {
                wireframeMode = !wireframeMode;

                if (wireframeMode)
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
                showToolTip = !showToolTip;
            }
            AddTooltip("You can turn off tooltips here."); // Add tooltip for UI element above.

            ImGui::EndMenu();
        }
        AddTooltip("Tweak editor settings."); // Add tooltip for UI element above.

        if (ImGui::BeginMenu("Window"))
        {
            if (ImGui::MenuItem("Rendering Stats Window"))
            {
                showStats = !showStats;
            }
            AddTooltip("Shows how many textures are created, how many are used, etc."); // Add tooltip for UI element above.

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

            isPlaying = true;
        }
        AddTooltip("Play the scene."); // Add tooltip for UI element above.

        // Pause scene button.
        ImGui::SameLine();
        if (ImGui::ImageButton((void*)pauseButton, ImVec2(ImGui::GetContentRegionMax().x / 120, ImGui::GetContentRegionAvail().y)))
        {
            isPlaying = false;
        }
        AddTooltip("Pause the scene."); // Add tooltip for UI element above.

        // Stop scene button.
        ImGui::SameLine();
        if (ImGui::ImageButton((void*)stopButton, ImVec2(ImGui::GetContentRegionMax().x / 120, ImGui::GetContentRegionAvail().y)))
        {

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

    std::shared_ptr<AudioManager> audioManager = renderer.objectsToRender[selectedObject]->GetComponent<AudioManager>();

    ImGui::Text(std::to_string(audioManager->sounds.size()).c_str()); // Shows the number of sounds in the vector.

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

        audioManager->sounds.push_back(s); // Adds a sound to the list.
    }
    AddTooltip("Create a new sound effect."); // Add tooltip for UI element above.

    // Loops through every item in sounds vector.
    for (int i = 0; i < audioManager->sounds.size(); i++)
    {
        ImGui::Indent();

        ImGui::PushID(i);
        Sound sound = audioManager->sounds[i]; // Gets a sound from the vector.

        if (sound.soundName != audioManager->sounds[i].soundName) ImGui::SetNextItemOpen(true);

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

                ImGui::InputText("##name", &audioManager->sounds[i].soundName);

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);

                ImGui::Text("File:");

                ImGui::TableSetColumnIndex(1);

                ImGui::Text(audioManager->sounds[i].filePath.c_str());
                ImGui::SameLine();

                // Creates a button for the user to select a file through the file explorer.
                if (ImGui::ImageButton((void*)miniFolderIcon, ImVec2(20, 20), ImVec2(0, 1), ImVec2(1, 0)))
                {
                    audioManager->GenSound(i); // If a new sound has been created, generate sources and buffers for audio to play.
                    audioManager->Stop(audioManager->sounds[i].soundName); // Stops the audio source.

                    const char* filterTypes[1] = { "*.wav" }; // Defines the file filters.
                    std::string filePath = fileManager.OpenFileExplorer(filterTypes, "Select a sound file", rootPath); // Gets the file path using the file explorer.

                    // Checks if file path is valid.
                    if (!filePath.empty())
                    {
                        audioManager->sounds[i].filePath = filePath.erase(0, rootPath.length() + 1); // Updates the file path.

                        audioManager->sounds[i].audioSource->ChangeFile(audioManager->sounds[i].filePath); // Changes the file path for the audio source.
                        audioManager->sounds[i].audioSource->SetProperties(audioManager->sounds[i].pitch, audioManager->sounds[i].volume, audioManager->sounds[i].isLooping, audioManager->sounds[i].position, audioManager->sounds[i].velocity); // Resets audio source properties.
                    }
                }
                AddTooltip("Locate the file through your files window."); // Adds a tooltip to the UI element above.

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);

                ImGui::Text("Pitch:");

                ImGui::TableSetColumnIndex(1);

                ImGui::SliderFloat("##pitch", &audioManager->sounds[i].pitch, 0.1, 2.0, "%.2f");

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);

                ImGui::Text("Volume:");

                ImGui::TableSetColumnIndex(1);

                ImGui::SliderFloat("##volume", &audioManager->sounds[i].volume, 0.0, 1.0, "%.2f");

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

                    ImGui::InputFloat("##PX", &audioManager->sounds[i].position.x, 0.0f, 0.0f, "%.f");
                    ImGui::PopStyleColor();

                    ImGui::TableSetColumnIndex(1);
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 1.0f, 0.3f, 1.0f)); // Y color

                    ImGui::SetCursorPosY(ImGui::GetCursorPos().y - 3);
                    ImGui::Text("Y");

                    ImGui::SameLine();
                    ImGui::InputFloat("##PY", &audioManager->sounds[i].position.y, 0.0f, 0.0f, "%.f");
                    ImGui::PopStyleColor();

                    ImGui::TableSetColumnIndex(2);

                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 0.3f, 1.0f, 1.0f)); // Z color

                    ImGui::SetCursorPosY(ImGui::GetCursorPos().y - 3);
                    ImGui::Text("Z");

                    ImGui::SameLine();
                    ImGui::InputFloat("##PZ", &audioManager->sounds[i].position.z, 0.0f, 0.0f, "%.f");
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

                    ImGui::InputFloat("##VX", &audioManager->sounds[i].velocity.x, 0.0f, 0.0f, "%.f");
                    ImGui::PopStyleColor();

                    ImGui::TableSetColumnIndex(1);

                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 1.0f, 0.3f, 1.0f)); // Y color

                    ImGui::SetCursorPosY(ImGui::GetCursorPos().y - 3);
                    ImGui::Text("Y");

                    ImGui::SameLine();
                    ImGui::InputFloat("##VY", &audioManager->sounds[i].velocity.y, 0.0f, 0.0f, "%.f");
                    ImGui::PopStyleColor();

                    ImGui::TableSetColumnIndex(2);

                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 0.3f, 1.0f, 1.0f)); // Z color

                    ImGui::SetCursorPosY(ImGui::GetCursorPos().y - 3);
                    ImGui::Text("Z");

                    ImGui::SameLine();
                    ImGui::InputFloat("##VZ", &audioManager->sounds[i].velocity.z, 0.0f, 0.0f, "%.f");
                    ImGui::PopStyleColor();

                    ImGui::EndTable();
                }

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);

                ImGui::Text("Play On Start-Up:");
                AddTooltip("Should the sound effect play when the scene starts?");

                ImGui::TableSetColumnIndex(1);

                ImGui::Checkbox("##playStart", &audioManager->sounds[i].playOnStartUp);

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);

                ImGui::Text("Repeat Delay:");
                AddTooltip("Should the sound effect not overlap another? This can be useful for voicelines.");

                ImGui::TableSetColumnIndex(1);

                ImGui::Checkbox("##repeat", &audioManager->sounds[i].repeatDelay);

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);

                ImGui::Text("Looped:");
                AddTooltip("Should the sound effect play constantly?");

                ImGui::TableSetColumnIndex(1);

                ImGui::Checkbox("##loop", &audioManager->sounds[i].isLooping);

                ImGui::NewLine();
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);

                ImGui::Text("Test:");

                ImGui::TableSetColumnIndex(1);

                // The play button.
                if (ImGui::Button("Play"))
                {
                    audioManager->GenSound(i); // If a new sound has been created, generate sources and buffers for audio to play.

                    audioManager->sounds[i].audioSource->Stop(); // Stops the sound from playing.

                    audioManager->sounds[i].audioSource->ChangeFile(audioManager->sounds[i].filePath); // Changes the file path for the audio source.
                    audioManager->sounds[i].audioSource->SetProperties(audioManager->sounds[i].pitch, audioManager->sounds[i].volume, audioManager->sounds[i].isLooping, audioManager->sounds[i].position, audioManager->sounds[i].velocity); // Resets audio source properties.
                    
                    audioManager->sounds[i].audioSource->Play(); // Plays the sound.
                }

                ImGui::SameLine();

                // The stop button.
                if (ImGui::Button("Pause"))
                {
                    audioManager->sounds[i].audioSource->Pause(); // Stops the sound.
                }

                ImGui::SameLine();

                // The stop button.
                if (ImGui::Button("Stop"))
                {
                    audioManager->sounds[i].audioSource->Stop(); // Stops the sound.
                }

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
    for (auto& obj : renderer.objectsToRender)
    {
        camera = obj->GetComponent<Camera>();
        cameraObj = obj;

        /*if (camera != nullptr)
        {

        }*/
    }
}

void Editor::AddTooltip(const char* message) const
{
    if (!showToolTip) return; // Don't show a tooltip if it's been disabled.
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
