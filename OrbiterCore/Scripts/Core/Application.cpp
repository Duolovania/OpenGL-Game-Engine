#include "Core/application.h"

#include "Rendering/indexbuffer.h"
#include "Rendering/vertexarray.h"

#include "Rendering/shader.h"
#include "stb_image.h"

float timeTime = 0, oldTimeSinceStart = 0, timeSinceStart, deltaTime;
int actionIndex = 0, keyBindIndex = 0;

bool applicationQuit = false, listenToInput = false, showStats = false;

std::unique_ptr<FrameBuffer> framebuffer;
std::unique_ptr<Shader> fbShader;

Engine Engine::instance;

// Application starting point.
void Application::Run()
{
    Init(m_screenWidth, m_screenHeight, "Orbiter Editor");

    Core.renderingLayer->framebuffer = std::make_unique<FrameBuffer>(m_screenWidth, m_screenHeight);
    Core.renderingLayer->framebuffer->Gen();

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

    // Create a windowed mode window and its OpenGL context 
    window = glfwCreateWindow(screenWidth, screenHeight, windowTitle, NULL, NULL);

    GLFWimage images[1];
    images[0].pixels = stbi_load("../OrbiterCore/Res/orbitlogo.png", &images[0].width, &images[0].height, 0, 4);
    glfwSetWindowIcon(window, 1, images);
    glfwSetKeyCallback(window, Engine::HandleInput);

    if (!window)
        glfwTerminate();

    // Make the window's context current
    glfwMakeContextCurrent(window);
    glfwMaximizeWindow(window); // Sets window to fullscreen by default.

    int status = gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    if (!status)
        std::cout << "Error: gladInit non-operational";

    glfwSwapInterval(1);

    std::cout << glGetString(GL_VERSION) << std::endl;

    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    Core.renderingLayer->Init(window);
    Core.audioManager = std::make_unique<AudioManager>();

    Sound newSound;
    newSound.soundName = "Test";
    newSound.filePath = "Assets/SFX/elf-singing-89296.wav";

    Core.audioManager->sounds.push_back(newSound);
    Core.audioManager->GenSounds();
}

// Application loop.
void Application::Loop()
{   
    /*applicationQuit = !Core.renderingLayer.Begin();

    Core.renderingLayer.End();*/

    Core.renderingLayer->OnUpdate(deltaTime);

    if (Core.InputManager.GetActionStrength("arrowUp")) Core.audioManager->Play("Test");
    if (Core.InputManager.GetActionStrength("arrowDown")) Core.audioManager->Pause("Test");

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
    Core.audioManager->KillAudioManager();
    Core.renderingLayer->Close();

    glfwTerminate();

    Core.renderingLayer->CleanUp();
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