#include "Headers/application.h"

#include "Headers/texture.h"
#include "Headers/Renderer.h"

#include "Headers/IndexBuffer.h"
#include "Headers/VertexBuffer.h"

#include "Headers/VertexArray.h"
#include "Headers/VertexBufferLayout.h"

#include "Headers/shader.h"
#include "glm/gtc/matrix_transform.hpp"

#include <fstream>
#include <sstream>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 600

float vertex = 1.0f;

float positions[] =
{
    -vertex, -vertex, 0.0f, 0.0f, // 0
    vertex, -vertex, 1.0f, 0.0f, // 1
    vertex, vertex, 1.0f, 1.0f, // 2
    -vertex, vertex, 0.0f, 1.0f, // 3
};

unsigned int indices[] =
{
    0, 1, 2,
    2, 3, 0
};

VertexArray va;
VertexBuffer vb(positions, 4 * 4 * sizeof(float));
IndexBuffer ib(indices, 6);
//Texture texture("Assets/Sprites/ForestE.png");
Texture texture("Res/Textures/namjas.JPG");

float red = 0.0f, green = 1.0f, blue = 1.0f, alpha = 1.0f, increment = 1.0f;
int location;

float camX = 0.0, camY = 0.0;

Shader shader("Res/Shaders/Basic.shader");
Renderer renderer;

void HandleInput(GLFWwindow* window, int key, int scanCode, int action, int mods);

// Application starting point.
void Application::Run()
{
    Init(SCREEN_WIDTH, SCREEN_HEIGHT, "Test Window");

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

    va.Gen();
    ib.Gen(indices, 6);
    
    /*VertexBuffer vb(positions, 4 * 4 * sizeof(float));*/

    vb.Gen(positions);
    VertexBufferLayout layout;

    layout.Push<float>(2);
    layout.Push<float>(2);

    va.AddBuffer(vb, layout);

    glm::mat4 proj = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -1.0f, 1.0f); // Orthrographic projection.
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)); // Camera translation.

    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0, -2.0, 0)); // Model translation

    glm::mat4 mvp = proj * view * model;

    shader.CreateShader();
    shader.Bind();

    //shader.SetUniform4f("u_Color", 0.2f, 0.3f, 0.8f, 1.0f);
    shader.SetUniformMat4f("u_MVP", mvp);

    texture.Gen();
    texture.Bind();
    shader.SetUniform1i("u_Texture", 0);

    va.Unbind();
    shader.UnBind();
    vb.Unbind();
    ib.Unbind();

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
    // Render here 
    renderer.Clear();

    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();

    shader.Bind();

    glm::mat4 proj = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -1.0f, 1.0f);
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(-camX, -camY, 0)); // Camera translation.

    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)); // Model translation

    glm::mat4 mvp = proj * view * model;
    shader.SetUniform4f("u_Color", red, green, blue, alpha);
    shader.SetUniformMat4f("u_MVP", mvp);

    vb.Bind();
    vb.ModData(positions);

    renderer.Draw(va, ib, shader);

    if (red > 1.0f)
        increment = -0.025f;
    else if (red < 0.7f)
        increment = 0.025f;

    red += increment;

    ImGui::Begin("Rect Properties");

    ImGui::Text("Control RGB values of shader");
    ImGui::SliderFloat("G:", &green, 0.0f, 1.0f, "%.1f");
    ImGui::SliderFloat("B:", &blue, 0.0f, 1.0f, "%.1f");
    ImGui::SliderFloat("A:", &alpha, 0.0f, 1.0f, "%.1f");

    ImGui::SliderFloat("X:", &camX, -2.0f, 2.0f, "%.3f");
    ImGui::SliderFloat("Y:", &camY, -2.0f, 2.0f, "%.3f");

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Swap front and back buffers 
    glfwSwapBuffers(window);

    // Poll for and process events 
    glfwPollEvents();
}

// Terminates program.
void Application::Close()
{
    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
}

// Handles key input actions.
void HandleInput(GLFWwindow* window, int key, int scanCode, int action, int mods)
{
    switch (key)
    {
        case GLFW_KEY_D:
            break;
        case GLFW_KEY_W:
            positions[0] += 0.1f;
            break;
        case GLFW_KEY_A:
            break;
        case GLFW_KEY_S:
            break;
    }
}