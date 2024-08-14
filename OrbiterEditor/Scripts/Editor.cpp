#include "Headers/editor.h"

#include <filesystem>

FileManager fileManager;
std::string inputString, searchTerm, rootPath;
std::string currentPath = "C:/Users/Ryhan Khan/Downloads/GitHub/OpenGL-Engine/OpenGL/OrbiterEditor/Assets";

testSpace::Test* currentTest = nullptr;
testSpace::TestMenu* testMenu = new testSpace::TestMenu(currentTest);

enum UISelect
{
    None,
    ActionButton,
    KeybindButton
};

UISelect uiSelect = UISelect::None;

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

    rootPath = currentPath;
}

void Editor::OnUpdate(float deltaTime)
{
    GLCall(glClearColor(0.05f, 0.05f, 0.05f, 1.0f));

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
                    }
                }
            }

            if (ImGui::MenuItem("Exit Editor"))
            {
                //return false;
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

            if (ImGui::MenuItem("Editor Settings"))
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

    if (ImGui::Button("New file"))
    {
        fileManager.CreateFile("Assets/Scenes/bazinga", ".froggie");
    }

    if (ImGui::Button("Print file"))
    {
        fileManager.LoadFile("Assets/Scenes/bazinga", ".froggie");
    }

    /*if (selectedObject > -1)
    {
        ImGui::Text("Object Name:");
        ImGui::SameLine();
        ImGui::InputText("##label0", &renderer.objectsToRender[selectedObject].objectName);

        if (ImGui::CollapsingHeader("Transform"))
        {
            ImGui::Text("Position:");
            ImGui::SameLine();
            ImGui::InputFloat2("##label1", (float*)&renderer.objectsToRender[selectedObject].transform.position, "%.f");

            ImGui::Text("Scale:");
            ImGui::SameLine();
            ImGui::InputFloat2("##label2", (float*)&renderer.objectsToRender[selectedObject].transform.scale.x, "%.f");

            ImGui::Text("Rotation:");
            ImGui::SameLine();
            ImGui::InputFloat("##label3", &renderer.objectsToRender[selectedObject].transform.rotation.z, 0.0f, 0.0f, "%.f");
        }

        if (ImGui::CollapsingHeader("Sprite Renderer"))
        {
            ImGui::Image((void*)renderer.objectsToRender[selectedObject].texture, ImVec2(200, 200), ImVec2(0, 1), ImVec2(1, 0));

            ImGui::Text("Colour:");
            ImGui::SameLine();
            ImGui::ColorEdit4("##label4", renderer.objectsToRender[selectedObject].color);

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
        ImGui::InputText("##label0", &cameraName);

        if (ImGui::CollapsingHeader("Transform"))
        {
            ImGui::Text("Position:");
            ImGui::SameLine();
            ImGui::InputFloat2("##label1", (float*)&camPos, "%.f");
        }
    }*/

    ImGui::End();

    ImGui::Begin("Hierarchy");

    /*if (ImGui::Button((cameraName).c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 0)))
    {
        selectedObject = -1;
    }

    for (int i = 0; i < renderer.objectsToRender.size(); i++)
    {
        if (ImGui::Button((renderer.objectsToRender[i].objectName).c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 0)))
        {
            selectedObject = i;
        }
    }*/

    ImGui::End();

    GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("Viewport");

    ImVec2 viewportSize = ImGui::GetContentRegionAvail();
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

    framebuffer->Bind();

    GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    if (currentTest)
    {
        currentTest->OnUpdate(deltaTime);
        currentTest->OnRender(glm::ortho(((float)viewportSize.x / (float)viewportSize.y) * -100, ((float)viewportSize.x / (float)viewportSize.y) * 100, -100.0f, 100.0f, -1.0f, 1.0f));

        framebuffer->UnBind();

        ImGui::Begin("Debug Log");
        currentTest->OnImGuiRender();
        ImGui::End();

        if (showStats)
        {
            ImGui::Begin("Rendering Stats");

            ImGui::Text("Textures Loaded: %.0f", double(currentTest->GetStats()[1]));
            ImGui::Text("New Textures Created: %.0f", double(currentTest->GetStats()[0]));

            ImGui::End();
        }
    }

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