#include "Core/application.h"

#include "Rendering/indexbuffer.h"
#include "Rendering/vertexarray.h"

#include "Rendering/shader.h"
#include "Core/editorui.h"
#include "stb_image.h"

#include "testclearcolour.h"
#include "testtexture2d.h"

float timeTime = 0, oldTimeSinceStart = 0, timeSinceStart, deltaTime;
int actionIndex = 0, keyBindIndex = 0;

bool applicationQuit = false, listenToInput = false, showStats = false;

testSpace::Test* currentTest = nullptr;
testSpace::TestMenu* testMenu = new testSpace::TestMenu(currentTest);

std::unique_ptr<FrameBuffer> framebuffer;
std::unique_ptr<Shader> fbShader;

Engine Engine::instance;
EditorUI editor;

// Application starting point.
void Application::Run()
{
    Init(m_screenWidth, m_screenHeight, "Orbiter Editor");

    framebuffer = std::make_unique<FrameBuffer>(m_screenWidth, m_screenHeight);
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

    GLFWimage images[1];
    images[0].pixels = stbi_load("Res/orbitlogo.png", &images[0].width, &images[0].height, 0, 4);
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

    currentTest = testMenu;
    testMenu->RegisterTest<testSpace::TestClearColour>("Clear Colour");
    testMenu->RegisterTest<testSpace::TestTexture2D>("Texture 2D");

    editor.Init(window);

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
    GLCall(glClearColor(0.05f, 0.05f, 0.05f, 1.0f));
    
    applicationQuit = !editor.Begin();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("Viewport");

    ImVec2 viewportSize = ImGui::GetContentRegionAvail();
    framebuffer->Resize(glm::vec2(viewportSize.x, viewportSize.y));

    ImGui::Image((void*)framebuffer->GetTexture(), viewportSize, ImVec2(0, 1), ImVec2(1, 0));
    ImVec2 imagePos = ImGui::GetCursorScreenPos();

    ImVec2 position = ImVec2(imagePos.x * 1.15f, imagePos.y / 10.15f);
    ImGui::SetCursorScreenPos(position);

    if (editor.showFPS)
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

        if (Core.InputManager.GetActionStrength("arrowUp")) Core.audioManager->Play("Test");
        if (Core.InputManager.GetActionStrength("arrowDown")) Core.audioManager->Pause("Test");
    }

    editor.End();

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