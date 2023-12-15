#include "Headers/application.h"
#include "Headers/texture.h"
#include "Headers/Renderer.h"
#include "Headers/IndexBuffer.h"
#include "Headers/VertexBuffer.h"
#include "Headers/VertexArray.h"
#include "Headers/VertexBufferLayout.h"
#include <fstream>
#include <sstream>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 600

// Struct contains Vertex and Fragment data.
struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
};

// Searches .shader files for vertex and fragment data.
static ShaderProgramSource ParseShader(const std::string& filePath)
{
    std::ifstream stream(filePath);

    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;

    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
                type = ShaderType::VERTEX;
            else if (line.find("fragment") != std::string::npos)
                type = ShaderType::FRAGMENT;
        }
        else
        {
            ss[(int)type] << line << '\n';
        }
    }

    // Returns vertex and fragment data
    return { ss[0].str(), ss[1].str() };
}

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    GLCall(glShaderSource(id, 1, &src, nullptr));
    GLCall(glCompileShader(id));

    int result;
    GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));

    // Error handler
    if (result == GL_FALSE)
    {
        int length;
        GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
        char* message = (char*) _malloca(length * sizeof(char));

        GLCall(glGetShaderInfoLog(id, length, &length, message));
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
        std::cout << message << std::endl;

        GLCall(glDeleteShader(id));
        return 0;
    }

    return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    GLCall(glAttachShader(program, vs));
    GLCall(glAttachShader(program, fs));

    GLCall(glLinkProgram(program));
    GLCall(glValidateProgram(program));

    GLCall(glDeleteShader(vs));
    GLCall(glDeleteShader(fs));

    return program;
}

float vertex = 0.05f;

float positions[] =
{
    -0.5f, -0.5f,
    0.5f, -0.5f,
    0.5f, 0.5f,
    -0.5f, 0.5f,
};

unsigned int indices[] =
{
    0, 1, 2,
    2, 3, 0
};

unsigned int vao;
VertexArray va;
IndexBuffer ib(indices, 6);

float red = 0.0f, increment = 1.0f;
int location;

unsigned int shader; // Create shader

void HandleInput(GLFWwindow* window, int key, int scanCode, int action, int mods);

// Application starting point.
void Application::Run()
{
    Init(SCREEN_WIDTH, SCREEN_HEIGHT, "Test Window");

    /*Texture texture("../Assets/Sprites/B happy.png");
    texture.Bind();*/

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
        Loop();
    }

    glDeleteProgram(shader);
    Close();
}

// Initializer.
void::Application::Init(int screenWidth, int screenHeight, const char* windowTitle)
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

    va.Gen();
    ib.Gen(indices, 6);

    GLCall(glGenVertexArrays(1, &vao));
    GLCall(glBindVertexArray(vao));
    
    VertexBuffer vb(positions, 4 * 2 * sizeof(float));

    VertexBufferLayout layout;
    layout.Push<float>(2);
    va.AddBuffer(vb, layout);
    
    ShaderProgramSource source = ParseShader("Res/Shaders/Basic.shader");

    shader = CreateShader(source.VertexSource, source.FragmentSource); // Assign vertex and fragment to shader.
    GLCall(glUseProgram(shader));

    GLCall(location = glGetUniformLocation(shader, "u_Color"));
    ASSERT(location != -1);
    GLCall(glUniform4f(location, 0.2f, 0.3f, 0.8f, 1.0f));

    va.Unbind();

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
    GLCall(glClear(GL_COLOR_BUFFER_BIT));

    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();

    GLCall(glUseProgram(shader));
    GLCall(glUniform4f(location, red, 0.3f, 0.8f, 1.0f));

    va.Bind();
    ib.Bind();
    GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

    if (red > 1.0f)
        increment = -0.05f;
    else if (red < 0.0f)
        increment = 0.05f;

    red += increment;

    ImGui::Begin("Rect Properties");
    ImGui::Text("Control properties of rect texture.");
    //ImGui::ColorEdit3("Rect Color", color);
    ImGui::SliderFloat("Move Vertex", &vertex, 0.05f, 0.5f);
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
    /*switch (key)
    {
        case GLFW_KEY_D:
            break;
        case GLFW_KEY_W:
            break;
        case GLFW_KEY_A:
            break;
        case GLFW_KEY_S:
            break;
    }*/
}