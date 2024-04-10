#include "Headers/application.h"

#include "Headers/texture.h"
#include "Headers/renderer.h"

#include "Headers/indexbuffer.h"
#include "Headers/vertexBuffer.h"

#include "Headers/vertexarray.h"
#include "Headers/vertexbufferlayout.h"

#include "Headers/shader.h"
#include "glm/gtc/matrix_transform.hpp"

#include "Tests/testclearcolour.h"
#include "Tests/testtexture2d.h"

#include <fstream>
#include <sstream>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 600

float timeTime = 0, oldTimeSinceStart = 0, timeSinceStart, deltaTime;
Renderer renderer;

testSpace::Test* currentTest = nullptr;
testSpace::TestMenu* testMenu = new testSpace::TestMenu(currentTest);

void HandleInput(GLFWwindow* window, int key, int scanCode, int action, int mods);

// Application starting point.
void Application::Run()
{
    Init(SCREEN_WIDTH, SCREEN_HEIGHT, "Nam Nam Grabbers");

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
        Loop();
    }

    Close();
}

// Initializer.
void Application::Init(int screenWidth, int screenHeight, const char* windowTitle)
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a windowed mode window and its OpenGL context 
    window = glfwCreateWindow(screenWidth, screenHeight, windowTitle, NULL, NULL);
    glfwSetKeyCallback(window, HandleInput);

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
    io.FontDefault = io.Fonts->AddFontFromFileTTF("Assets/Fonts/Moon2.0-Bold.otf", 16.0f);

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

        ImGui::Begin("Testis");

        if (currentTest != testMenu && ImGui::Button("<-"))
        {
            delete currentTest;
            currentTest = testMenu;
        }

        currentTest->OnImGuiRender();
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
void HandleInput(GLFWwindow* window, int key, int scanCode, int action, int mods)
{
    if (currentTest)
    {
        currentTest->OnHandleInput(window, key, scanCode, action, mods);
    }
}