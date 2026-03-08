#include "Core/application.h"

#include "Rendering/indexbuffer.h"
#include "Rendering/vertexarray.h"

#include "Rendering/shader.h"
#include "stb_image.h"
#include "Core/filemanager.h"
#include <windows.h>
#include <filesystem>

float timeTime = 0, oldTimeSinceStart = 0, timeSinceStart, deltaTime;
bool applicationQuit = false, showConsole = false;

Engine Engine::instance;

void Application::Run()
{
    FileManager fileManager;
    std::string windowTitle;

    // Ensures that only projects are loaded for the editor and game.
    if (applicationType != OBApplicationType::LauncherOB)
    {
        std::string projectPath = fileManager.LoadLaunchInstructions(std::filesystem::current_path().parent_path().string() + "\\OrbiterCore\\launchinstructions.instructOB").selectedProjPath;
        if (projectPath == "") Close(); // Terminates the program if no project was found.

        // Loads the project config.
        Project = fileManager.LoadProjectConfig(projectPath + "\\" + "Game1" + ".projectOB");

        // Fix the project path if the config file is corrupted.
        if (Project.projectPath.empty()) Project.projectPath = projectPath; // Sets the path of the 'Assets' folder.

        // Fix the project assets path if the config file is corrupted.
        if (Project.assetsPath.empty()) Project.assetsPath = Project.projectPath + "\\Assets"; // Sets the path of the 'Assets' folder.

        // Fix the project name if the config file is corrupted.
        if (Project.name.empty())
        {
            std::filesystem::path filePath = Project.assetsPath;
            std::string projectFolderName = filePath.parent_path().filename().string(); // Gets the name of the project folder. This should match the project name either way.

            Project.name = projectFolderName;
        }
    }

    // Sets the title for the window.
    switch (applicationType)
    {
        case OBApplicationType::LauncherOB:
            windowTitle = "Orbiter Launcher";
            
            break;
        case OBApplicationType::EditorOB:
            windowTitle = "Orbiter Editor - " + Project.name;
            break;
        default:
            windowTitle = Project.name;
            Core.m_applicationState = OBApplicationState::Play;
            break;
    }

    // Hides the console window by default.
    ::ShowWindow(::GetConsoleWindow(), SW_HIDE);

    // Initializes the window.
    Init(m_screenWidth, m_screenHeight, windowTitle.c_str());

    //// Sets up script controller and attaches scripts.
    //Core.m_scriptController.Init();
    //Core.m_scriptController.AddScript(Project.filePath + "\\Scripts\\testscript.lua");
    //Core.m_scriptController.AddScript(Project.filePath + "\\Scripts\\testscripttwo.lua");

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
    images[0].pixels = stbi_load("../OrbiterEditor/res/Application Icons/orbitlogo.png", &images[0].width, &images[0].height, 0, 4);
    glfwSetWindowIcon(window, 1, images);

    glfwSetKeyCallback(window, Engine::HandleInput);
    glfwSetScrollCallback(window, Engine::HandleScrollInput);

    if (!window)
        glfwTerminate();

    // Makes this the current window.
    glfwMakeContextCurrent(window);
    if (applicationType != OBApplicationType::LauncherOB) glfwMaximizeWindow(window); // Sets window to fullscreen by default.

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

    if (showConsole)
    {
        ::ShowWindow(::GetConsoleWindow(), SW_SHOW);
    }
    else
    {
        ::ShowWindow(::GetConsoleWindow(), SW_HIDE);
    }

    // Checks if the game is playing.
    if (Core.m_applicationState == OBApplicationState::Play)
    {
        // Checks if the game hasn't started yet.
        if (!hasStarted)
        {
            //Core.m_scriptController.CallStart(); // Calls all 'Start' functions.
            hasStarted = true; // Stops start from being called.
        }

        //Core.m_scriptController.CallUpdate(); // Calls all 'Update' functions.
    }
    else if (Core.m_applicationState == OBApplicationState::Stop)
    {
        hasStarted = false; // Allows 'Start' functions to run again.
    }

    // Calculates the deltaTime.
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
    Core.renderingLayer->Close(); // Closes the rendering layer.
    glfwTerminate(); // Terminates the window.
}

void Application::SetScreenResolution(int screenWidth, int screenHeight)
{
    m_screenWidth = screenWidth;
    m_screenHeight = screenHeight;
}

void Engine::ToggleConsoleWindow()
{
    showConsole = !showConsole;
}

bool Engine::GetConsoleWindowState()
{
    return showConsole;
}

void Engine::HandleInput(GLFWwindow* window, int key, int scanCode, int action, int mods)
{
    // Loops through each input action.
    for (int i = 0; i < Project.InputManager.actionList.size(); i++)
    {
        // Loops through each keybind.
        for (int j = 0; j < Project.InputManager.actionList[i].GetKeyBinds().size(); j++)
        {
            // Checks if the keybind is pressed.
            if (Project.InputManager.actionList[i].GetKeyBindIndex(j) == key)
            {
                Project.InputManager.actionList[i].SetStrength(action); // Triggers the keybind.
            }
        }
    }

    // Checks if the input manager is listening for input.
    if (Project.InputManager.listenToInput)
    {
        Project.InputManager.actionList[Project.InputManager.selectedAction].AddKeyBind(key); // Adds the keybind to the selected action.
        Project.InputManager.listenToInput = false; // Resets the listening state.
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