#include "Core/application.h"

#include "Rendering/texture.h"
#include "Core/renderer.h"

#include "Rendering/indexbuffer.h"
#include "Rendering/vertexBuffer.h"

#include "Rendering/vertexarray.h"
#include "Rendering/vertexbufferlayout.h"

#include "Rendering/framebuffer.h"

#include "Rendering/shader.h"
#include "gtc/matrix_transform.hpp"

#include "testclearcolour.h"
#include "testtexture2d.h"

#include <vector>

#include <fstream>
#include <sstream>

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

float timeTime = 0, oldTimeSinceStart = 0, timeSinceStart, deltaTime;
int actionIndex = 0, keyBindIndex = 0;

bool applicationQuit = false, listenToInput = false;
static char inputString[10];

bool wireframeMode = false, showStats = false;
Renderer renderer;

testSpace::Test* currentTest = nullptr;

testSpace::TestMenu* testMenu = new testSpace::TestMenu(currentTest);
Engine Engine::instance;

std::unique_ptr<FrameBuffer> framebuffer;
std::unique_ptr<Shader> fbShader;

ImVec2 viewportSize;

enum UISelect
{
    None,
    ActionButton,
    KeybindButton
};

UISelect uiSelect = UISelect::None;

// Application starting point.
void Application::Run()
{
    Init(SCREEN_WIDTH, SCREEN_HEIGHT, "Engine");

    framebuffer = std::make_unique<FrameBuffer>(SCREEN_WIDTH, SCREEN_HEIGHT);
    framebuffer->Gen();


    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window) && !applicationQuit)
    {
        Loop();
    }

    Close();
}

// Initializer.
void Application::Init(int screenWidth, int screenHeight, const char* windowTitle)
{
    if (!glfwInit())
        std::cout << "Error: glfwInit non-operational";

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_DECORATED, false);

    // Create a windowed mode window and its OpenGL context 
    window = glfwCreateWindow(screenWidth, screenHeight, windowTitle, NULL, NULL);
    glfwSetKeyCallback(window, Engine::HandleInput);

    if (!window)
        glfwTerminate();

    // Make the window's context current
    glfwMakeContextCurrent(window);

    int status = gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    if (!status)
        std::cout << "Error: gladInit non-operational";

    glfwSwapInterval(1);

    std::cout << glGetString(GL_VERSION) << std::endl;

    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    currentTest = testMenu;
    testMenu->RegisterTest<testSpace::TestClearColour>("Clear Colour");
    testMenu->RegisterTest<testSpace::TestTexture2D>("Texture 2D");

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.FontDefault = io.Fonts->AddFontFromFileTTF("Assets/Fonts/scada/Scada-Regular.ttf", 18.0f);

    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

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

// Application loop.
void Application::Loop()
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
                applicationQuit = true;
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
                    ImGui::InputText("Enter Action Name:", inputString, IM_ARRAYSIZE(inputString));

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
            if (ImGui::MenuItem("Documentation"))
            {

            }

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    ImGui::Begin("Assets Folder");

    ImGui::End();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("Viewport");


    viewportSize = ImGui::GetContentRegionAvail();
    framebuffer->Resize(glm::vec2(viewportSize.x, viewportSize.y));

    ImGui::Image((void*)framebuffer->GetTexture(), viewportSize, ImVec2(0, 1), ImVec2(1, 0));
    ImVec2 imagePos = ImGui::GetCursorScreenPos();

    ImVec2 position = ImVec2(imagePos.x + 20, imagePos.y - 625);
    ImGui::SetCursorScreenPos(position);

    if (currentTest != testMenu)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.988f, 0.659f, 0.176f, 1.0f));
        ImGui::Text("FPS: %.1f", double(1.0f / ImGui::GetIO().DeltaTime));
        ImGui::PopStyleColor();
    }

    ImGui::End();
    ImGui::PopStyleVar();

    framebuffer->Bind();
    renderer.Clear();

    if (currentTest)
    {
        currentTest->OnUpdate(deltaTime);

        currentTest->OnRender(glm::ortho(((float)viewportSize.x / (float)viewportSize.y) * -100, ((float)viewportSize.x / (float)viewportSize.y) * 100, -100.0f, 100.0f, -1.0f, 1.0f));
        framebuffer->UnBind();


        ImGui::Begin("Debug Log");

        currentTest->OnImGuiRender();

        if (ImGui::Button("Input Settings"))
        {
            ImGui::OpenPopup("Input");
        }

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

    timeSinceStart = static_cast<float>(glfwGetTime());
    deltaTime = timeSinceStart - oldTimeSinceStart;
    oldTimeSinceStart = timeSinceStart;

    // Swap front and back buffers 
    glfwSwapBuffers(window);

    // Poll for and process events 
    glfwPollEvents();
    timeTime += (float) 0.1;

}

// Terminates program.
void Application::Close()
{
    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();

    delete currentTest;
    if (currentTest != testMenu) delete testMenu;
}

// Handles key input actions.
void Engine::HandleInput(GLFWwindow* window, int key, int scanCode, int action, int mods)
{
    for (int i = 0; i < Core.InputManager.actionList.size(); i++)
    {
        for (int j = 0; j < Core.InputManager.actionList[i].GetKeyBinds().size(); j++)
        {
            if (Core.InputManager.actionList[i].GetKeyBindIndex(j) == key)
            {
                Core.InputManager.actionList[i].SetStrength(action);
            }
        }
    }

    if (listenToInput)
    {
        Core.InputManager.actionList[actionIndex].AddKeyBind(key);
        listenToInput = false;
    }
}