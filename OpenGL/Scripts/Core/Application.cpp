#include "Core/application.h"

#include "Rendering/texture.h"
#include "Core/renderer.h"

#include "Rendering/indexbuffer.h"
#include "Rendering/vertexBuffer.h"

#include "Rendering/vertexarray.h"
#include "Rendering/vertexbufferlayout.h"

#include "Rendering/shader.h"
#include "gtc/matrix_transform.hpp"

#include "testclearcolour.h"
#include "testtexture2d.h"

#include <fstream>
#include <sstream>

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

float timeTime = 0, oldTimeSinceStart = 0, timeSinceStart, deltaTime;
int actionIndex = 0, keyBindIndex = 0;

bool applicationQuit = false, listenToInput = false;
static char inputString[10];

Renderer renderer;

testSpace::Test* currentTest = nullptr;
testSpace::TestMenu* testMenu = new testSpace::TestMenu(currentTest);
Engine Engine::instance;

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

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_DECORATED, false);

    // Create a windowed mode window and its OpenGL context 
    window = glfwCreateWindow(screenWidth, screenHeight, windowTitle, NULL, NULL);
    glfwSetKeyCallback(window, Engine::HandleInput);

    if (!window)
        glfwTerminate();

    // Make the window's context current
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK)
        std::cout << "Error: glewInit non-operational";

    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    currentTest = testMenu;
    testMenu->RegisterTest<testSpace::TestClearColour>("Clear Colour");
    testMenu->RegisterTest<testSpace::TestTexture2D>("Texture 2D");

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.FontDefault = io.Fonts->AddFontFromFileTTF("Assets/Fonts/Chivo/Chivo-Light.ttf", 16.0f);

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

// Application loop.
void Application::Loop()
{
    GLCall(glClearColor(0.05f, 0.05f, 0.05f, 1.0f));

    // Render here 
    renderer.Clear();

    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();

    if (currentTest)
    {
        currentTest->OnUpdate(deltaTime);
        currentTest->OnRender();

        ImGui::Begin("Main");

        if (currentTest != testMenu && ImGui::Button("<-"))
        {
            delete currentTest;
            currentTest = testMenu;
        }

        currentTest->OnImGuiRender();

        if (ImGui::Button("Input Settings"))
        {
            ImGui::OpenPopup("Input");
        }







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

            if (ImGui::ListBoxHeader("Actions"))
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

                ImGui::ListBoxFooter();
            }

            if (ImGui::Button("Save and Close")) ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
        }







        if (ImGui::Button("Quit to Desktop")) applicationQuit = true;

        ImGui::End();
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