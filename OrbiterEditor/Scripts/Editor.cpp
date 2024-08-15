#include "Headers/editor.h"

#include <filesystem>

FileManager fileManager;
int selectedObject = -2;
std::string inputString, searchTerm, rootPath;
std::string currentPath = "C:/Users/Ryhan Khan/Downloads/GitHub/OpenGL-Engine/OpenGL/OrbiterEditor/Assets";

testSpace::Test* currentTest = nullptr;
testSpace::TestMenu* testMenu = new testSpace::TestMenu(currentTest);

std::unique_ptr<Shader> fbShader;

float sprintSpeed;

enum UISelect
{
    None,
    ActionButton,
    KeybindButton
};

UISelect uiSelect = UISelect::None;

void Editor::Init(GLFWwindow* window)
{
    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    currentTest = testMenu;
    testMenu->RegisterTest<testSpace::TestClearColour>("Clear Colour");
    testMenu->RegisterTest<testSpace::TestTexture2D>("Texture 2D");

    renderer.Init();

    fbShader = std::make_unique<Shader>("../OrbiterCore/Res/Shaders/Framebuffer.shader");
    fbShader->CreateShader();
    fbShader->Bind();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    StylesConfig();

    rootPath = currentPath;
}

bool Editor::OnUpdate(float deltaTime)
{

    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();

    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_None);

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
                return false;
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Project"))
        {
            if (ImGui::MenuItem("Project Settings"))
            {

                ImGui::Begin("Project Settings");
                ImGui::OpenPopup("Input");

                if (ImGui::BeginPopupModal("Input", NULL))
                {
                    ImGui::InputText("Enter Action Name:", &inputString);

                    if (ImGui::Button("Add"))
                    {
                        Core.InputManager.AddAction(Action(inputString));
                    }

                    ImGui::SameLine();

                    if (ImGui::Button("Delete"))
                    {
                        switch (uiSelect)
                        {
                        case UISelect::ActionButton:
                            LOG(actionIndex);
                            Core.InputManager.DeleteAction(actionIndex);
                            break;
                        case UISelect::KeybindButton:
                            LOG(keyBindIndex);
                            Core.InputManager.actionList[actionIndex].DeleteKeyBind(keyBindIndex);
                            break;
                        default:
                            break;
                        }
                    }

                    ImGui::SameLine();

                    if (ImGui::Button("Listen to Input")) listenToInput = true;

                    if (listenToInput)
                    {
                        ImGui::SameLine();
                        ImGui::Text("Listening...");
                    }

                    if (ImGui::BeginListBox("Actions"))
                    {
                        for (int i = 0; i < Core.InputManager.actionList.size(); i++)
                        {
                            if (ImGui::Selectable(("Name: " + Core.InputManager.actionList[i].GetActionName()).c_str()), actionIndex == i)
                            {
                                actionIndex = i;

                                uiSelect = UISelect::ActionButton;
                            }

                            for (int j = 0; j < Core.InputManager.actionList[i].GetKeyBinds().size(); j++)
                            {
                                if (ImGui::Selectable(Core.InputManager.actionList[i].GetKeyName(j)), keyBindIndex == j)
                                {
                                    actionIndex = i;
                                    keyBindIndex = j;

                                    uiSelect = UISelect::KeybindButton;
                                }
                            }
                        }

                        ImGui::EndListBox();
                    }

                    if (ImGui::Button("Save and Close")) ImGui::CloseCurrentPopup();
                    ImGui::EndPopup();
                }
                ImGui::End();
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

        ImGui::EndMainMenuBar();
    }

    ImGui::Begin("Assets Folder");

    ImGui::Text("Search");
    ImGui::SameLine();
    ImGui::InputText("##label8", &searchTerm);

    ImGui::SameLine();
    if (ImGui::Button("Back"))
    {
        currentPath = rootPath;
    }

    namespace fs = std::filesystem;

    for (const auto& entry : fs::directory_iterator(currentPath))
    {
        if (entry.is_directory())
        {
            if (ImGui::Selectable(entry.path().filename().string().c_str()))
            {
                currentPath = std::string(entry.path().string());
            }
        }
        else
        {
            if (strstr(entry.path().filename().string().c_str(), searchTerm.c_str()) != nullptr) ImGui::Selectable(entry.path().filename().string().c_str());
        }
    }

    ImGui::End();

    ImGui::Begin("Inspector");

    if (selectedObject > -1)
    {
        ImGui::Text("Object Name:");
        ImGui::SameLine();
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
            ImGui::Image((void*) renderer.objectsToRender[selectedObject].texture, ImVec2(200, 200), ImVec2(0, 1), ImVec2(1, 0));

            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 1.0f, 0.3f, 0.5f));
            ImGui::Text(renderer.objectsToRender[selectedObject].m_imagePath.c_str());
            ImGui::PopStyleColor();

            ImGui::Text("Colour:");
            ImGui::SameLine();
            ImGui::ColorEdit4("##label4", (float*) &renderer.objectsToRender[selectedObject].color);

            ImGui::SameLine();
            if (ImGui::Button("Reset"))
            {
                renderer.objectsToRender[selectedObject].SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
            }
        }

    }
    else if (selectedObject == -1)
    {
        ImGui::Text("Object Name:");
        ImGui::SameLine();
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
                ImGui::PopItemWidth();

                ImGui::EndTable();
            }
        }

        if (ImGui::CollapsingHeader("Camera Output"))
        {
            ImGui::Image((void*)framebuffer->GetTexture(), ImVec2(200, 200), ImVec2(0, 1), ImVec2(1, 0));

            ImGui::Text("Output Colour:");
            ImGui::SameLine();
            ImGui::ColorEdit4("##label7", (float*) &camera2D.outputColor);

            ImGui::SameLine();
            if (ImGui::Button("Reset##1"))
            {
                camera2D.SetColor(camera2D.outputColor, { 1.0f, 1.0f, 1.0f, 1.0f });
            }

            ImGui::Text("Background Colour:");
            ImGui::SameLine();
            ImGui::ColorEdit3("##label8", (float*) &camera2D.backgroundColor);

            ImGui::SameLine();
            if (ImGui::Button("Reset##2"))
            {
                camera2D.SetColor(camera2D.backgroundColor, { 0.05f, 0.05f, 0.05f, 1.0f });
            }
        }
    }

    ImGui::End();

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

    GLCall(glClearColor(camera2D.backgroundColor[0], camera2D.backgroundColor[1], camera2D.backgroundColor[2], 1.0f));
    GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("Viewport");

    viewportSize = ImGui::GetContentRegionAvail();
    framebuffer->Resize(glm::vec2(viewportSize.x, viewportSize.y));

    ImGui::Image((void*)framebuffer->GetTexture(), viewportSize, ImVec2(0, 1), ImVec2(1, 0));
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

    fbShader->Bind();
    framebuffer->Bind();

    if (currentTest)
    {
        currentTest->OnUpdate(deltaTime);
        currentTest->OnRender();

        if (currentTest != testMenu)
        {
            sprintSpeed = Core.InputManager.GetActionStrength("sprint") * 150;
            camera2D.transform.position += Vector2(Core.InputManager.BasicMovement().x * (100.0f + sprintSpeed) * deltaTime, Core.InputManager.BasicMovement().y * (100.0f + sprintSpeed) * deltaTime);

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
    }

    framebuffer->UnBind();
    fbShader->UnBind();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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
    }
}
