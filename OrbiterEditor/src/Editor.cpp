#include "editor.h"

#include <filesystem>

FileManager fileManager;
int selectedObject = -2;
std::string inputString, searchTerm, rootPath;
std::string currentPath = "C:/Users/Ryhan Khan/Downloads/GitHub/OpenGL-Engine/OpenGL/OrbiterEditor/Assets";

testSpace::Test* currentTest = nullptr;
testSpace::TestMenu* testMenu = new testSpace::TestMenu(currentTest);

float sprintSpeed;
Sound tempSound;
bool soundChanged = false, newSound = false, showProjSettings = false;

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
unsigned int playButton, pauseButton, stopButton, fileIcon, folderIcon, wavFileIcon, fontFileIcon, sceneFileIcon, imageFileIcon;

void Editor::Init(GLFWwindow* window)
{
    currentTest = testMenu;
    testMenu->RegisterTest<testSpace::TestClearColour>("Clear Colour");
    testMenu->RegisterTest<testSpace::TestTexture2D>("Texture 2D");

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
    fbShader->SetUniform1i("screenTexture", 0);

    frameBufferVA = std::make_unique<VertexArray>();
    frameBufferVA->Gen();
    frameBufferVA->Bind();

    frameBufferVB = std::make_unique<VertexBuffer>(sizeof(rectangleVertices));
    frameBufferVB->Gen(&rectangleVertices);

    VertexBufferLayout layout;
    layout.Push<float>(2);
    layout.Push<float>(2);

    frameBufferVA->AddBuffer(*frameBufferVB, layout);

    rootPath = currentPath;
    iconTextures = std::make_unique<Texture>("../OrbiterCore/Res/Application Icons/playbutton.png");
    iconTextures->Gen(true);

    playButton = iconTextures->GetBufferID();
    pauseButton = iconTextures->Load("../OrbiterCore/Res/Application Icons/pausebutton.png", true);
    stopButton = iconTextures->Load("../OrbiterCore/Res/Application Icons/stopbutton.png", true);

    folderIcon = iconTextures->Load("../OrbiterCore/Res/Application Icons/foldericon.png", true);
    fileIcon = iconTextures->Load("../OrbiterCore/Res/Application Icons/fileicon.png", true);

    wavFileIcon = iconTextures->Load("../OrbiterCore/Res/Application Icons/wavfileicon.png", true);
    fontFileIcon = iconTextures->Load("../OrbiterCore/Res/Application Icons/fontfileicon.png", true);
    sceneFileIcon = iconTextures->Load("../OrbiterCore/Res/Application Icons/scenefileicon.png", true);
    imageFileIcon = iconTextures->Load("../OrbiterCore/Res/Application Icons/imagefileicon.png", true);
}

bool Editor::OnUpdate(float deltaTime)
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

    GLCall(glClearColor(camera2D.backgroundColor[0], camera2D.backgroundColor[1], camera2D.backgroundColor[2], 1.0f));
    GLCall(glClear(GL_COLOR_BUFFER_BIT));

    if (currentTest)
    {
        currentTest->OnUpdate(deltaTime);
        currentTest->OnRender();

        if (currentTest != testMenu)
        {
            sprintSpeed = Core.InputManager.GetActionStrength("sprint") * 150;

            camera2D.transform.scale = Vector3(viewportSize.x, viewportSize.y, 0);
            camera2D.transform.position += Vector2(Core.InputManager.BasicMovement().x * (100.0f + sprintSpeed) * deltaTime, Core.InputManager.BasicMovement().y * (100.0f + sprintSpeed) * deltaTime);
            Core.audioManager->sounds[0].audioSource->SetProperties(1, 1, false, glm::vec3(renderer.objectsToRender[1].transform.position.x - camera2D.transform.position.x, renderer.objectsToRender[1].transform.position.y - camera2D.transform.position.y, 0.0f));

            renderer.Draw(glm::ortho(((float)viewportSize.x / (float)viewportSize.y) * -100, ((float)viewportSize.x / (float)viewportSize.y) * 100, -100.0f, 100.0f, -1.0f, 1.0f), camera2D.GetView(), {camera2D.outputColor[0], camera2D.outputColor[1], camera2D.outputColor[2], camera2D.outputColor[3]});
        }

        ImGui::Begin("Debug Log");
        currentTest->OnImGuiRender();
        
        if (ImGui::Button("New file"))
        {
            fileManager.CreateFile("Assets/Scenes/bazinga", ".froggie");
        }

        if (ImGui::Button("Print file"))
        {
            fileManager.LoadFile("Assets/Scenes/bazinga", ".froggie");
        }

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
    delete currentTest;
    if (currentTest != testMenu) delete testMenu;
}

void Editor::StylesConfig()
{
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.FontDefault = io.Fonts->AddFontFromFileTTF("../OrbiterCore/Res/Fonts/scada/Scada-Regular.ttf", 18.0f);

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

void Editor::Hierarchy()
{
    ImGui::Begin("Hierarchy");

    if (currentTest != testMenu)
    {
        if (ImGui::Button((camera2D.objectName).c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 0)))
        {
            selectedObject = -1;
        }

        for (int i = 0; i < renderer.objectsToRender.size(); i++)
        {
            if (ImGui::Button((renderer.objectsToRender[i].objectName).c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 0)))
            {
                selectedObject = i;
            }
        }
    }
    ImGui::End();
}

void Editor::Inspector()
{
    ImGui::Begin("Inspector");

    if (selectedObject > -1)
    {
        ImGui::InputText("##label0", &renderer.objectsToRender[selectedObject].objectName);

        ImGui::SameLine();
        if (ImGui::Button("Jump To"))
        {
            camera2D.transform.position = renderer.objectsToRender[selectedObject].transform.position;
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

                ImGui::InputFloat("##PX", &renderer.objectsToRender[selectedObject].transform.position.x, 0.0f, 0.0f, "%.f");
                ImGui::PopStyleColor();

                ImGui::TableSetColumnIndex(2);
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 1.0f, 0.3f, 1.0f)); // Y color

                ImGui::SetCursorPosY(ImGui::GetCursorPos().y - 3);
                ImGui::Text("Y");

                ImGui::SameLine();
                ImGui::InputFloat("##PY", &renderer.objectsToRender[selectedObject].transform.position.y, 0.0f, 0.0f, "%.f");
                ImGui::PopStyleColor();

                ImGui::TableSetColumnIndex(3);

                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 0.3f, 1.0f, 1.0f)); // Z color

                ImGui::SetCursorPosY(ImGui::GetCursorPos().y - 3);
                ImGui::Text("Z");

                ImGui::SameLine();
                ImGui::InputFloat("##PZ", &renderer.objectsToRender[selectedObject].transform.position.z, 0.0f, 0.0f, "%.f");
                ImGui::PopStyleColor();


                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);

                ImGui::Text("Rotation:");

                ImGui::TableSetColumnIndex(1);
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.3f, 0.3f, 1.0f)); // X color

                ImGui::Text("X");

                ImGui::SameLine();
                ImGui::SetCursorPosY(ImGui::GetCursorPos().y - 3);

                ImGui::InputFloat("##RX", &renderer.objectsToRender[selectedObject].transform.rotation.x, 0.0f, 0.0f, "%.f");
                ImGui::PopStyleColor();

                ImGui::TableSetColumnIndex(2);

                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 1.0f, 0.3f, 1.0f)); // Y color

                ImGui::SetCursorPosY(ImGui::GetCursorPos().y - 3);
                ImGui::Text("Y");

                ImGui::SameLine();
                ImGui::InputFloat("##RY", &renderer.objectsToRender[selectedObject].transform.rotation.y, 0.0f, 0.0f, "%.f");
                ImGui::PopStyleColor();

                ImGui::TableSetColumnIndex(3);

                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 0.3f, 1.0f, 1.0f)); // Z color

                ImGui::SetCursorPosY(ImGui::GetCursorPos().y - 3);
                ImGui::Text("Z");

                ImGui::SameLine();
                ImGui::InputFloat("##RZ", &renderer.objectsToRender[selectedObject].transform.rotation.z, 0.0f, 0.0f, "%.f");
                ImGui::PopStyleColor();

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);

                ImGui::Text("Scale:");
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.3f, 0.3f, 1.0f)); // X color

                ImGui::TableSetColumnIndex(1);

                ImGui::Text("X");

                ImGui::SameLine();
                ImGui::SetCursorPosY(ImGui::GetCursorPos().y - 3);

                ImGui::InputFloat("##SX", &renderer.objectsToRender[selectedObject].transform.scale.x, 0.0f, 0.0f, "%.f");
                ImGui::PopStyleColor();

                ImGui::TableSetColumnIndex(2);

                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 1.0f, 0.3f, 1.0f)); // Y color

                ImGui::SetCursorPosY(ImGui::GetCursorPos().y - 3);
                ImGui::Text("Y");

                ImGui::SameLine();
                ImGui::InputFloat("##SY", &renderer.objectsToRender[selectedObject].transform.scale.y, 0.0f, 0.0f, "%.f");
                ImGui::PopStyleColor();

                ImGui::TableSetColumnIndex(3);

                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 0.3f, 1.0f, 1.0f)); // Z color

                ImGui::SetCursorPosY(ImGui::GetCursorPos().y - 3);
                ImGui::Text("Z");

                ImGui::SameLine();
                ImGui::InputFloat("##SZ", &renderer.objectsToRender[selectedObject].transform.scale.z, 0.0f, 0.0f, "%.f");
                ImGui::PopStyleColor();

                ImGui::PopItemWidth(); // Reset item width

                /*std::array<Vector3, 3> thigny = { renderer.objectsToRender[selectedObject].transform.position, renderer.objectsToRender[selectedObject].transform.rotation, renderer.objectsToRender[selectedObject].transform.scale };
                CreateTransformColumn({ "Position", "Rotation", "Scale" }, thigny);*/

                ImGui::EndTable();
            }
        }

        if (ImGui::CollapsingHeader("Sprite Renderer"))
        {
            ImGui::Image((void*)renderer.objectsToRender[selectedObject].cTexture.textureBuffer, ImVec2(200, 200), ImVec2(0, 1), ImVec2(1, 0));

            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 1.0f, 0.3f, 0.5f));
            ImGui::Text(renderer.objectsToRender[selectedObject].cTexture.m_imagePath.c_str());
            ImGui::PopStyleColor();

            ImGui::Text("Colour:");
            ImGui::SameLine();
            ImGui::ColorEdit4("##label4", (float*)&renderer.objectsToRender[selectedObject].color);

            ImGui::SameLine();
            if (ImGui::Button("Reset"))
            {
                renderer.objectsToRender[selectedObject].SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
            }
        }

    }
    else if (selectedObject == -1)
    {
        ImGui::InputText("##label0", &camera2D.objectName);

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

                ImGui::InputFloat("##PX", &camera2D.transform.position.x, 0.0f, 0.0f, "%.f");
                ImGui::PopStyleColor();

                ImGui::TableSetColumnIndex(2);
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 1.0f, 0.3f, 1.0f)); // Y color

                ImGui::SetCursorPosY(ImGui::GetCursorPos().y - 3);
                ImGui::Text("Y");

                ImGui::SameLine();
                ImGui::InputFloat("##PY", &camera2D.transform.position.y, 0.0f, 0.0f, "%.f");
                ImGui::PopStyleColor();

                ImGui::TableSetColumnIndex(3);

                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 0.3f, 1.0f, 1.0f)); // Z color

                ImGui::SetCursorPosY(ImGui::GetCursorPos().y - 3);
                ImGui::Text("Z");

                ImGui::SameLine();
                ImGui::InputFloat("##PZ", &camera2D.transform.position.z, 0.0f, 0.0f, "%.f");
                ImGui::PopStyleColor();


                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);

                ImGui::Text("Rotation:");

                ImGui::TableSetColumnIndex(1);
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.3f, 0.3f, 1.0f)); // X color

                ImGui::Text("X");

                ImGui::SameLine();
                ImGui::SetCursorPosY(ImGui::GetCursorPos().y - 3);

                ImGui::InputFloat("##RX", &camera2D.transform.rotation.x, 0.0f, 0.0f, "%.f");
                ImGui::PopStyleColor();

                ImGui::TableSetColumnIndex(2);

                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 1.0f, 0.3f, 1.0f)); // Y color

                ImGui::SetCursorPosY(ImGui::GetCursorPos().y - 3);
                ImGui::Text("Y");

                ImGui::SameLine();
                ImGui::InputFloat("##RY", &camera2D.transform.rotation.y, 0.0f, 0.0f, "%.f");
                ImGui::PopStyleColor();

                ImGui::TableSetColumnIndex(3);

                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 0.3f, 1.0f, 1.0f)); // Z color

                ImGui::SetCursorPosY(ImGui::GetCursorPos().y - 3);
                ImGui::Text("Z");

                ImGui::SameLine();
                ImGui::InputFloat("##RZ", &camera2D.transform.rotation.z, 0.0f, 0.0f, "%.f");
                ImGui::PopStyleColor();


                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);

                ImGui::Text("Scale:");

                ImGui::TableSetColumnIndex(1);
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.3f, 0.3f, 1.0f)); // X color

                ImGui::Text("X");

                ImGui::SameLine();
                ImGui::SetCursorPosY(ImGui::GetCursorPos().y - 3);

                ImGui::InputFloat("##SX", &camera2D.transform.scale.x, 0.0f, 0.0f, "%.f");
                ImGui::PopStyleColor();

                ImGui::TableSetColumnIndex(2);

                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 1.0f, 0.3f, 1.0f)); // Y color

                ImGui::SetCursorPosY(ImGui::GetCursorPos().y - 3);
                ImGui::Text("Y");

                ImGui::SameLine();
                ImGui::InputFloat("##SY", &camera2D.transform.scale.y, 0.0f, 0.0f, "%.f");
                ImGui::PopStyleColor();

                ImGui::TableSetColumnIndex(3);

                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 0.3f, 1.0f, 1.0f)); // Z color

                ImGui::SetCursorPosY(ImGui::GetCursorPos().y - 3);
                ImGui::Text("Z");

                ImGui::SameLine();
                ImGui::InputFloat("##SZ", &camera2D.transform.scale.z, 0.0f, 0.0f, "%.f");
                ImGui::PopStyleColor();
                ImGui::PopItemWidth();

                ImGui::EndTable();
            }
        }

        if (ImGui::CollapsingHeader("Camera Output"))
        {
            ImGui::Image((void*)framebuffer->GetTexture(), ImVec2(200, 200), ImVec2(0, 1), ImVec2(1, 0));

            ImGui::Text("Output Colour:");
            ImGui::SameLine();
            ImGui::ColorEdit4("##label7", (float*)&camera2D.outputColor);

            ImGui::SameLine();
            if (ImGui::Button("Reset##1"))
            {
                camera2D.SetColor(camera2D.outputColor, { 1.0f, 1.0f, 1.0f, 1.0f });
            }

            ImGui::Text("Background Colour:");
            ImGui::SameLine();
            ImGui::ColorEdit3("##label8", (float*)&camera2D.backgroundColor);

            ImGui::SameLine();
            if (ImGui::Button("Reset##2"))
            {
                camera2D.SetColor(camera2D.backgroundColor, { 0.05f, 0.05f, 0.05f, 1.0f });
            }
        }

        if (ImGui::CollapsingHeader("AudioManager"))
        {

            ImGui::Text("Sounds: ");
            ImGui::SameLine();

            ImGui::Text(std::to_string(Core.audioManager->sounds.size()).c_str());
            ImGui::SameLine();
            if (ImGui::Button("+"))
            {
                Sound s;

                s.soundName = "Enter sound name";
                s.filePath = "Enter path to .wav files";
                s.volume = 1;
                s.pitch = 1;
                s.position = glm::vec3(0);
                s.velocity = glm::vec3(0);

                Core.audioManager->sounds.push_back(s);

                newSound = true;
                soundChanged = false;
            }

            for (auto& sound : Core.audioManager->sounds)
            {
                ImGui::Indent();

                if (ImGui::CollapsingHeader(sound.soundName.c_str()))
                {
                    if (!soundChanged)
                    {
                        tempSound = sound;
                        soundChanged = true;
                    }

                    ImGui::Indent();
                    ImGui::Text("Name:");
                    ImGui::SameLine();
                    ImGui::InputText("##name", &tempSound.soundName);

                    ImGui::SameLine();
                    if (ImGui::Button("Save"))
                    {
                        sound = tempSound;
                        soundChanged = false;
                    }

                    ImGui::SameLine();
                    if (ImGui::Button("Undo"))
                    {
                        soundChanged = false;
                    }

                    ImGui::Text("File:");
                    ImGui::SameLine();
                    ImGui::InputText("##path", &tempSound.filePath);

                    if (ImGui::BeginDragDropTarget())
                    {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ITEM_DRAG"))
                        {
                            // Process payload
                            std::string* data = (std::string*)payload->Data;
                            // Do something with the data
                            tempSound.filePath = *data;
                            std::cout << data << std::endl;
                        }
                        ImGui::EndDragDropTarget();
                    }

                    ImGui::Text("Pitch:");
                    ImGui::SameLine();
                    ImGui::SliderFloat("##pitch", &tempSound.pitch, 0.1f, 2, "%.2f");

                    ImGui::Text("Volume:");
                    ImGui::SameLine();
                    ImGui::SliderFloat("##volume", &tempSound.volume, 0, 1, "%.2f");

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

                        ImGui::InputFloat("##PX", &tempSound.position.x, 0.0f, 0.0f, "%.f");
                        ImGui::PopStyleColor();

                        ImGui::TableSetColumnIndex(2);
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 1.0f, 0.3f, 1.0f)); // Y color

                        ImGui::SetCursorPosY(ImGui::GetCursorPos().y - 3);
                        ImGui::Text("Y");

                        ImGui::SameLine();
                        ImGui::InputFloat("##PY", &tempSound.position.y, 0.0f, 0.0f, "%.f");
                        ImGui::PopStyleColor();

                        ImGui::TableSetColumnIndex(3);

                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 0.3f, 1.0f, 1.0f)); // Z color

                        ImGui::SetCursorPosY(ImGui::GetCursorPos().y - 3);
                        ImGui::Text("Z");

                        ImGui::SameLine();
                        ImGui::InputFloat("##PZ", &tempSound.position.z, 0.0f, 0.0f, "%.f");
                        ImGui::PopStyleColor();


                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);

                        ImGui::Text("Velocity:");

                        ImGui::TableSetColumnIndex(1);
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.3f, 0.3f, 1.0f)); // X color

                        ImGui::Text("X");

                        ImGui::SameLine();
                        ImGui::SetCursorPosY(ImGui::GetCursorPos().y - 3);

                        ImGui::InputFloat("##VX", &tempSound.velocity.x, 0.0f, 0.0f, "%.f");
                        ImGui::PopStyleColor();

                        ImGui::TableSetColumnIndex(2);

                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 1.0f, 0.3f, 1.0f)); // Y color

                        ImGui::SetCursorPosY(ImGui::GetCursorPos().y - 3);
                        ImGui::Text("Y");

                        ImGui::SameLine();
                        ImGui::InputFloat("##VY", &tempSound.velocity.y, 0.0f, 0.0f, "%.f");
                        ImGui::PopStyleColor();

                        ImGui::TableSetColumnIndex(3);

                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 0.3f, 1.0f, 1.0f)); // Z color

                        ImGui::SetCursorPosY(ImGui::GetCursorPos().y - 3);
                        ImGui::Text("Z");

                        ImGui::SameLine();
                        ImGui::InputFloat("##VZ", &tempSound.velocity.z, 0.0f, 0.0f, "%.f");
                        ImGui::PopStyleColor();

                        ImGui::EndTable();
                    }

                    ImGui::Text("Play On Start-Up:");
                    ImGui::SameLine();
                    ImGui::Checkbox("##playStart", &tempSound.playOnStartUp);

                    ImGui::Text("Repeat Delay:");
                    ImGui::SameLine();
                    ImGui::Checkbox("##repeat", &tempSound.repeatDelay);

                    ImGui::Text("Looped:");
                    ImGui::SameLine();
                    ImGui::Checkbox("##loop", &tempSound.isLooping);

                    if (ImGui::Button("Play"))
                    {
                        if (newSound) Core.audioManager->GenSounds();
                        newSound = false;

                        tempSound.audioSource->ChangeFile(tempSound.filePath);
                        tempSound.audioSource->SetProperties(tempSound.pitch, tempSound.volume, tempSound.isLooping, tempSound.position, tempSound.velocity);
                        tempSound.audioSource->Play();
                    }

                    ImGui::SameLine();
                    if (ImGui::Button("Stop"))
                    {
                        tempSound.audioSource->Stop();
                    }

                    ImGui::Unindent();
                }

                ImGui::Unindent();
            }
        }
    }

    ImGui::End();
}

void Editor::ContentBrowser()
{
    ImGui::Begin("Assets Folder");

    ImGui::Text("Search");
    ImGui::SameLine();
    ImGui::InputText("##search", &searchTerm);

    namespace fs = std::filesystem;

    if (currentPath != rootPath)
    {
        ImGui::SameLine();

        if (ImGui::Button("Back"))
        {
            fs::path parentDir = currentPath;
            currentPath = parentDir.parent_path().string();

            if (currentPath.length() < rootPath.length()) currentPath = rootPath;
        }
    }

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 0.804, 0.137, 0.75f));
    ImGui::Text(currentPath.c_str());
    ImGui::PopStyleColor();

    int counter = 0;
    ImVec2 padding = ImVec2(1.075f, 1.075f);
    ImVec2 originalPos = ImVec2(ImGui::GetCursorPos().x + padding.x, ImGui::GetCursorPos().y + padding.y);

    for (const auto& entry : fs::directory_iterator(currentPath))
    {
        ImVec2 buttonSize = ImVec2(200, 200);
        ImVec2 buttonPos = ImVec2(originalPos.x + (counter * (buttonSize.x * padding.x)), originalPos.y); // Calculates the x position based on how many items there are.

        // Sets the text position to the center of the thumbnail (sets the text origin position to the center of the thumbnail and subtracts it by the amount of characters. The subtraction is to ensure that the text is centered regardless of it's length).
        ImVec2 textPos = ImVec2(originalPos.x + ((counter * (buttonSize.x * padding.x) + (((buttonSize.x / 2)) - (ImGui::CalcTextSize(entry.path().filename().string().c_str()).x) / 2))), originalPos.y + (buttonSize.y * padding.y)); 

        // Checks if the thumbnail will exceed the window size.
        if ((buttonPos.x + buttonSize.x > ImGui::GetContentRegionMax().x))
        {
            counter = 0; // Resets the x position.
            originalPos = ImVec2(originalPos.x, ImGui::GetCursorPos().y + ((buttonSize.y / 4) * padding.y)); // Calculates the y position based on the button size and padding amount.

            buttonPos = ImVec2(originalPos.x + (counter * (buttonSize.x * padding.x)), originalPos.y); // Recalculates the button position with the x position being reset.
            textPos = ImVec2(originalPos.x + ((counter * (buttonSize.x * padding.x) + (((buttonSize.x / 2)) - (ImGui::CalcTextSize(entry.path().filename().string().c_str()).x) / 2))), originalPos.y + (buttonSize.y * padding.y)); // Recalculates the text position with the x position being reset.
        }
        
        if (entry.is_directory())
        {
            ImGui::PushID(counter);
            ImGui::SetCursorPos(buttonPos);

            if (ImGui::ImageButton((void*)folderIcon, ImVec2(200, 200), ImVec2(0, 1), ImVec2(1, 0)))
            {
                currentPath = std::string(entry.path().string());
            }

            ImGui::SetCursorPos(textPos);

            ImGui::Text(entry.path().filename().string().c_str());
            ImGui::SameLine();
            ImGui::PopID();
        }
        else
        {
            if (strstr(entry.path().filename().string().c_str(), searchTerm.c_str()) != nullptr)
            {
                ImGui::PushID(counter);
                ImGui::SetCursorPos(buttonPos);

                unsigned int fileThumbnail = fileIcon;
                int fileNameLength = entry.path().filename().string().find('.');

                std::string tempFileName = entry.path().filename().string().substr(fileNameLength);

                if (tempFileName == ".wav")
                {
                    fileThumbnail = wavFileIcon;
                }
                else if (tempFileName == ".ttf" || tempFileName == ".otf")
                {
                    fileThumbnail = fontFileIcon;
                }
                else if (tempFileName == ".froggie")
                {
                    fileThumbnail = sceneFileIcon;
                }
                else if (tempFileName == ".png" || tempFileName == ".jpg")
                {
                    fileThumbnail = imageFileIcon;
                }

                if (ImGui::ImageButton((void*)fileThumbnail, ImVec2(200, 200), ImVec2(0, 1), ImVec2(1, 0)))
                {
                    if (ImGui::BeginDragDropSource())
                    {
                        std::string path = entry.path().filename().string();
                        ImGui::Text("Dragging");
                        ImGui::SetDragDropPayload("ITEM_DRAG", &path, sizeof(path)); // Set payload
                        ImGui::EndDragDropSource();
                    }
                };

                ImGui::SetCursorPos(textPos);

                ImGui::Text(entry.path().filename().string().c_str());
                ImGui::SameLine();
                ImGui::PopID();
            }
        }

        counter++;
    }

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

            }

            if (ImGui::MenuItem("Open Scene"))
            {

            }

            if (ImGui::MenuItem("Open Recent"))
            {

            }

            if (ImGui::MenuItem("Save"))
            {

            }

            if (ImGui::MenuItem("Save As"))
            {

            }

            if (currentTest != testMenu)
            {
                if (ImGui::MenuItem("Close Scene"))
                {
                    if (currentTest != testMenu)
                    {
                        delete currentTest;
                        currentTest = testMenu;
                        selectedObject = -2;
                    }
                }
            }

            if (ImGui::MenuItem("Exit Editor"))
            {
                applicationRunning = false;
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Project"))
        {
            if (ImGui::MenuItem("Project Settings"))
            {
                showProjSettings = !showProjSettings;
            }

            ImGui::EndMenu();
        }

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

            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Window"))
        {
            if (ImGui::MenuItem("Rendering Stats Window"))
            {
                showStats = !showStats;
            }

            ImGui::EndMenu();
        }

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
        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetContentRegionMax().x / 1.2f);

        if (ImGui::ImageButton((void*)playButton, ImVec2(ImGui::GetContentRegionMax().x / 120, ImGui::GetContentRegionAvail().y)))
        {

        }

        ImGui::SameLine();
        if (ImGui::ImageButton((void*)pauseButton, ImVec2(ImGui::GetContentRegionMax().x / 120, ImGui::GetContentRegionAvail().y)))
        {

        }

        ImGui::SameLine();
        if (ImGui::ImageButton((void*)stopButton, ImVec2(ImGui::GetContentRegionMax().x / 120, ImGui::GetContentRegionAvail().y)))
        {

        }

        ImGui::EndMainMenuBar();
    }
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
