#include "Headers/application.h"
#include "Headers/texture.h"
#include <fstream>
#include <sstream>

#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))

static void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

// Use ASSERT(GLLogCall()) to debug.
static bool GLLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGL Error] (" << error << "): " << function << " " << file << ":" << line << std::endl;
        return false;
    }

    return true;
}

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

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 600

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);

    // Error handler
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*) alloca(length * sizeof(char));

        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
        std::cout << message << std::endl;

        glDeleteShader(id);
        return 0;
    }

    return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);

    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

//float color[3] = { 1.0f, 0.2f, 0.2f };
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

unsigned int buffer; // buffer ID
unsigned int shader; // Create shader
unsigned int ibo; // Index buffer

void HandleInput(GLFWwindow* window, int key, int scanCode, int action, int mods);

// Application starting point.
void Application::Run()
{
    Init(SCREEN_WIDTH, SCREEN_HEIGHT, "Ranut has smol bols");

    Texture texture("../Assets/Sprites/B happy.png");
    texture.Bind();

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window))
        Loop();

    glDeleteProgram(shader);
    Close();
}

// Initializer.
void::Application::Init(int screenWidth, int screenHeight, const char* windowTitle)
{
    glfwInit();

    // Create a windowed mode window and its OpenGL context 
    window = glfwCreateWindow(screenWidth, screenHeight, windowTitle, NULL, NULL);
    glfwSetKeyCallback(window, HandleInput);

    if (!window)
        glfwTerminate();

    // Make the window's context current
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
        std::cout << "Error: glewInit non-operational";
    
    glGenBuffers(1, &buffer); // How many buffers and the ID
    glBindBuffer(GL_ARRAY_BUFFER, buffer); // Selects buffer
    glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), positions, GL_STATIC_DRAW); // Generates buffer data

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
    
    glGenBuffers(1, &ibo); // How many buffers and the ID
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo); // Selects buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW); // Generates buffer data

    ShaderProgramSource source = ParseShader("Res/Shaders/Basic.shader");

    shader = CreateShader(source.VertexSource, source.FragmentSource); // assign vertex and fragment to shader.
    glUseProgram(shader);

    //glBindBuffer(GL_ARRAY_BUFFER, 0); // Deselects buffer.

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.FontDefault = io.Fonts->AddFontFromFileTTF("Assets/Fonts/Moon2.0-Bold.otf", 16.0f);

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    /*Texture my_Sprite("Assets/Sprites/B happy.png");
    my_Sprite.Bind();*/
}

// Application loop.
void Application::Loop()
{
    // Render here 
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();

    // positions[0] = vertex;
    //glBindBuffer(GL_ARRAY_BUFFER, buffer); // Selects buffer
    //glBufferSubData(GL_ARRAY_BUFFER, 0, 8 * sizeof(float), positions); // Generates buffer data

    // Create triangle.
    //glDrawArrays(GL_QUADS, 0, 4); // Draws buffer
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

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
            positions[0] += vertex;
            positions[2] += vertex;
            positions[4] += vertex;
            positions[6] += vertex;
            break;
        case GLFW_KEY_W:
            positions[1] += vertex;
            positions[3] += vertex;
            positions[5] += vertex;
            positions[7] += vertex;
            break;
        case GLFW_KEY_A:
            positions[0] -= vertex;
            positions[2] -= vertex;
            positions[4] -= vertex;
            positions[6] -= vertex;
            break;
        case GLFW_KEY_S:
            positions[1] -= vertex;
            positions[3] -= vertex;
            positions[5] -= vertex;
            positions[7] -= vertex;
            break;
    }*/
}