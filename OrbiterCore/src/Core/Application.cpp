#include "Core/application.h"

#include "Rendering/indexbuffer.h"
#include "Rendering/vertexarray.h"

#include "Rendering/shader.h"
#include "stb_image.h"

float timeTime = 0, oldTimeSinceStart = 0, timeSinceStart, deltaTime;

bool applicationQuit = false;

Engine Engine::instance;

void Application::Run()
{
    Init(m_screenWidth, m_screenHeight, "Orbiter Editor");

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

    glfwSetJoystickCallback(Engine::HandleGamePadInput);

    if (!window)
        glfwTerminate();

    // Make the window's context current
    glfwMakeContextCurrent(window);
    glfwMaximizeWindow(window); // Sets window to fullscreen by default.

    int status = gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    if (!status)
        std::cout << "Error: gladInit non-operational";

    std::cout << glGetString(GL_VERSION) << std::endl;

    Core.renderingLayer->framebuffer = std::make_unique<FrameBuffer>(m_screenWidth, m_screenHeight);
    Core.renderingLayer->framebuffer->Gen();

    Core.audioManager = std::make_unique<AudioManager>();

    Sound newSound;
    newSound.soundName = "Test";
    newSound.filePath = "SFX/elf-singing-89296.wav";

    Core.audioManager->sounds.push_back(newSound);
    Core.audioManager->GenAllSounds();

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
    Core.audioManager->KillAudioManager();
    Core.renderingLayer->Close();

    glfwTerminate();

    Core.renderingLayer->CleanUp();
}

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

    if (Core.InputManager.listenToInput)
    {
        Core.InputManager.actionList[Core.InputManager.selectedAction].AddKeyBind(key);
        Core.InputManager.listenToInput = false;
    }
}

void Engine::HandleScrollInput(GLFWwindow* window, double xoffset, double yoffset)
{
    if (yoffset > 0)
        std::cout << "Scroll up" << std::endl;
    else if (yoffset < 0) 
        std::cout << "Scroll down" << std::endl;

    if (xoffset > 0)
        std::cout << "Scroll left" << std::endl;
    else if (xoffset < 0)
        std::cout << "Scroll right" << std::endl;
}

void Engine::HandleGamePadInput(int jid, int event)
{
    int buttonCount, selectedButton;
    const unsigned char* buttons = glfwGetJoystickButtons(jid, &buttonCount);

    if (buttons)
    {
        for (int i = 0; i < Core.InputManager.actionList.size(); i++)
        {
            for (int j = 0; j < Core.InputManager.actionList[i].GetKeyBinds().size(); j++)
            {
                if (Core.InputManager.actionList[i].GetKeyBindIndex(j) && buttons[i])
                {
                    //int action = (buttons[i] == GLFW_PRESS) ? 1 : 0;
                    Core.InputManager.actionList[i].SetStrength(buttons[i]);
                    selectedButton = buttons[i];
                }
            }
        }
    }

    if (Core.InputManager.listenToInput)
    {
        Core.InputManager.actionList[Core.InputManager.selectedAction].AddKeyBind(buttons[0]);
        Core.InputManager.listenToInput = false;
    }
}