#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <glew.h>
#include <glfw3.h>
#include <iostream>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 600
void HandleInput(GLFWwindow* window, int key, int scanCode, int action, int mods);

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Ranut has small bols", NULL, NULL);

    glfwSetKeyCallback(window, HandleInput);

    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) std::cout << "Error: glewInit non-operational";

    float positions[] = 
    { 
        0.2f, -0.5f,
        -0.5f, -0.5f,
        -0.5f, 0.5f,
        0.2f, 0.5f,
    };

    float color[3] = { 1.0f, 0.2f, 0.2f };

    
    unsigned int buffer;

    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), positions, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    io.FontDefault = io.Fonts->AddFontFromFileTTF("Assets/Fonts/Moon2.0-Bold.otf", 16.0f);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplGlfw_NewFrame();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();

        // Create triangle.
        glColor3f(color[0], color[1], color[2]); //sets color
        glDrawArrays(GL_QUADS, 0, 4);

        ImGui::Begin("Rect Properties");
        ImGui::Text("Control properties of rect texture.");
        ImGui::ColorEdit3("Rect Color", color);
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}

void HandleInput(GLFWwindow* window, int key, int scanCode, int action, int mods)
{
    switch (key)
    {
        case GLFW_KEY_SPACE:
            std::cout << "space pressed";
            break;
    }
}