#include "Core/application.h"

#include "Rendering/indexbuffer.h"
#include "Rendering/vertexarray.h"

#include "Rendering/shader.h"
#include "stb_image.h"
#include "Core/filemanager.h"
#include <filesystem>

float timeTime = 0, oldTimeSinceStart = 0, timeSinceStart, deltaTime;

bool applicationQuit = false;

Engine Engine::instance;

void Application::Run()
{
    FileManager fileManager;

    // Loads the project config.
    Core.selectedProject = fileManager.LoadProjectConfig(std::filesystem::current_path().parent_path().string() + "\\Projects\\" + "Game1" + "\\" + "Game1" + ".projectOB");

    // Fix the project file path if the config file is corrupted.
    if (Core.selectedProject.filePath.empty()) Core.selectedProject.filePath = std::filesystem::current_path().parent_path().string() + "\\Projects\\" + "Game1" + "\\Assets"; // Sets the path of the 'Assets' folder.
    
    // Fix the project name if the config file is corrupted.
    if (Core.selectedProject.name.empty())
    {
        std::filesystem::path filePath = Core.selectedProject.filePath;
        std::string projectFolderName = filePath.parent_path().filename().string(); // Gets the name of the project folder. This should match the project name either way.

        Core.selectedProject.name = projectFolderName;
    }

    // Sets the application window properties.
    std::string windowTitle = "Orbiter Editor - " + Core.selectedProject.name;
    Init(m_screenWidth, m_screenHeight, windowTitle.c_str());

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window) && !applicationQuit)
    {
        Loop();
    }

    Close();
}

void Application::Init(int screenWidth, int screenHeight, const char* windowTitle)
{
    if (!glfwInit())
        std::cout << "Error: glfwInit non-operational";

    // Sets the glfw version.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a windowed mode window and its OpenGL context 
    window = glfwCreateWindow(screenWidth, screenHeight, windowTitle, NULL, NULL);

    GLFWimage images[1];
    images[0].pixels = stbi_load("../OrbiterCore/Res/Application Icons/orbitlogo.png", &images[0].width, &images[0].height, 0, 4);
    glfwSetWindowIcon(window, 1, images);

    glfwSetKeyCallback(window, Engine::HandleInput);
    glfwSetScrollCallback(window, Engine::HandleScrollInput);

    if (!window)
        glfwTerminate();

    // Makes this the current window.
    glfwMakeContextCurrent(window);
    glfwMaximizeWindow(window); // Sets window to fullscreen by default.

    // Checks if glad is working.
    int status = gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    if (!status)
        std::cout << "Error: gladInit non-operational";

    std::cout << glGetString(GL_VERSION) << std::endl;

    // Generates the frame buffer and initializes the current rendering layer. 
    Core.renderingLayer->framebuffer = std::make_unique<FrameBuffer>(m_screenWidth, m_screenHeight);
    Core.renderingLayer->framebuffer->Gen();
    Core.renderingLayer->Init(window);
}

void Application::Loop()
{   
    applicationQuit = !Core.renderingLayer->OnUpdate(deltaTime, timeTime);

    timeSinceStart = static_cast<float>(glfwGetTime());
    deltaTime = timeSinceStart - oldTimeSinceStart;
    oldTimeSinceStart = timeSinceStart;

    // Swap front and back buffers 
    glfwSwapBuffers(window);
    glfwSwapInterval(Core.renderingLayer->vsyncEnabled);

    // Poll for and process events 
    glfwPollEvents();
    timeTime += (float) 0.1;
}

void Application::Close()
{
    Core.renderingLayer->Close();
    glfwTerminate(); // Terminates the window.
}

void Engine::HandleInput(GLFWwindow* window, int key, int scanCode, int action, int mods)
{
    // Loops through each input action.
    for (int i = 0; i < Core.InputManager.actionList.size(); i++)
    {
        // Loops through each keybind.
        for (int j = 0; j < Core.InputManager.actionList[i].GetKeyBinds().size(); j++)
        {
            // Checks if the keybind is pressed.
            if (Core.InputManager.actionList[i].GetKeyBindIndex(j) == key)
            {
                Core.InputManager.actionList[i].SetStrength(action); // Triggers the keybind.
            }
        }
    }

    // Checks if the input manager is listening for input.
    if (Core.InputManager.listenToInput)
    {
        Core.InputManager.actionList[Core.InputManager.selectedAction].AddKeyBind(key); // Adds the keybind to the selected action.
        Core.InputManager.listenToInput = false; // Resets the listening state.
    }
}

void Engine::HandleScrollInput(GLFWwindow* window, double xoffset, double yoffset)
{
    /*if (yoffset > 0)
        std::cout << "Scroll up" << std::endl;
    else if (yoffset < 0) 
        std::cout << "Scroll down" << std::endl;

    if (xoffset > 0)
        std::cout << "Scroll left" << std::endl;
    else if (xoffset < 0)
        std::cout << "Scroll right" << std::endl;*/
}