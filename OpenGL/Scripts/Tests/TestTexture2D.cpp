#include "testtexture2d.h"
#include "Headers/Renderer.h"

#include "Headers/shader.h"
#include "glm/gtc/matrix_transform.hpp"

glm::vec2 inputVector;
bool horizontal, vertical;

namespace testSpace
{
	TestTexture2D::TestTexture2D()
		: translationA(50, 50, 0), translationB(0, 50, 0), proj(glm::ortho(-100.0f, 100.0f, -75.0f, 75.0f, -1.0f, 1.0f))
	{
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

		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		va = std::make_unique<VertexArray>();
		va->Gen();

		ib = std::make_unique<IndexBuffer>(indices, 6);
		ib->Gen(indices, 6);

		vb = std::make_unique<VertexBuffer>(positions, 4 * 4 * sizeof(float));
		vb->Gen(positions);

		VertexBufferLayout layout;

		layout.Push<float>(2);
		layout.Push<float>(2);

		va->AddBuffer(*vb, layout);

		view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)); // Camera translation.
		model = glm::translate(glm::mat4(1.0f), translationA); // Model translation.

		mvp = proj * view * model;

		shader = std::make_unique<Shader>("Res/Shaders/Basic.shader");
		shader->CreateShader();
		shader->Bind();

		texture = std::make_unique<Texture>("Res/Textures/namjas.JPG");
		texture->Gen();
		texture->Bind();

		shader->SetUniform1i("u_Texture", 0);

		va->Unbind();
		shader->UnBind();
		vb->Unbind();
		ib->Unbind();
	}

	TestTexture2D::~TestTexture2D()
	{

	}

	void TestTexture2D::OnUpdate(float deltaTime)
	{
		translationB.x = ((cos((deltaTime) * 1)) * 10);
		translationB.y = ((sin((deltaTime) * 1)) * 10);

		camPos += glm::vec2(inputVector.x * 100.0f * deltaTime, inputVector.y * 100.0f * deltaTime);

		if (red > 1.0f)
			increment = -0.025f;
		else if (red < 0.7f)
			increment = 0.025f;

		red += increment;
		this->deltaTime = deltaTime;
	}

	void TestTexture2D::OnRender()
	{
		GLCall(glClearColor(0.05f, 0.05f, 0.05f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT));

		float positions[] =
		{
			-vertex, -vertex, 0.0f, 0.0f, // 0
			vertex, -vertex, 1.0f, 0.0f, // 1
			vertex, vertex, 1.0f, 1.0f, // 2
			-vertex, vertex, 0.0f, 1.0f, // 3
		};

		Renderer renderer;

		view = glm::translate(glm::mat4(1.0f), glm::vec3(-camPos.x, -camPos.y, 0)); // Camera translation.
		model = glm::translate(glm::mat4(1.0f), translationA); // Model translation.

		mvp = proj * view * model;

		shader->Bind();
		shader->SetUniform4f("u_Color", glm::vec4(red, green, blue, alpha));

		vb->Bind();
		vb->ModData(positions);

		shader->SetUniformMat4f("u_MVP", mvp);
		renderer.Draw(*va, *ib, *shader);

		model = glm::translate(glm::mat4(1.0f), translationB); // Model translation.
		mvp = proj * view * model;

		shader->SetUniformMat4f("u_MVP", mvp);
		renderer.Draw(*va, *ib, *shader);
	}

	void TestTexture2D::OnImGuiRender()
	{
		ImGui::Text("FPS %.1f FPS", double(ImGui::GetIO().Framerate));

		ImGui::Text("Control RGB values of shader");
		ImGui::SliderFloat("G:", &green, 0.0f, 1.0f, "%.1f");
		ImGui::SliderFloat("B:", &blue, 0.0f, 1.0f, "%.1f");
		ImGui::SliderFloat("A:", &alpha, 0.0f, 1.0f, "%.1f");

		ImGui::SliderFloat2("Camera Pos:", &camPos.x, -100.0f, 100.0f, "%.3f");
		ImGui::SliderFloat2("Nam Pos:", &translationA.x, -100.0f, 100.0f, "%.3f");
	}

	void TestTexture2D::OnHandleInput(GLFWwindow* window, int key, int scanCode, int action, int mods)
	{
		switch (key)
		{
			case GLFW_KEY_D:
				InputMap(inputVector.x, action, 1);
				break;
			case GLFW_KEY_W:
				InputMap(inputVector.y, action, 1);
				break;
			case GLFW_KEY_A:
				InputMap(inputVector.x, action, -1);
				break;
			case GLFW_KEY_S:
				InputMap(inputVector.y, action, -1);
				break;
		}
	}

	void TestTexture2D::InputMap(float& input, int action, float value)
	{
		if (action == GLFW_RELEASE)
		{
			input -= value;
		}
		else
		{
			input += value;
		}

		input = Clamp(input, -1, 1);
	}

	float TestTexture2D::Clamp(float var, float min, float max)
	{
		const float tempVar = (var > min) ? var : min;
		return (tempVar < max) ? tempVar : max;
	}
}