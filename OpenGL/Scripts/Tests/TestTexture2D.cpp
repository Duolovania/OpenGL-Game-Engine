#include "testtexture2d.h"
#include "Headers/Renderer.h"

#include "Headers/shader.h"
#include "glm/gtc/matrix_transform.hpp"
#include <array>


glm::vec2 inputVector, namVector, namPos;
bool horizontal, vertical;

float vertex = 50.0f;
float upVertex = vertex * 3;

struct Vec3
{
	float x, y, z;
};

struct Vec2
{
	float x, y;
};

struct Vec4
{
	float x, y, z, w;
};


struct Vertex
{
	Vec3 Position;
	Vec4 Color;
	Vec2 TextureCoords;
	float TextureID;
};

static std::array<Vertex, 4> CreateQuad(float x, float y, float texID, Vec4 color)
{
	float size = 100.0f;

	Vertex v0;
	v0.Position = { x, y, 0.0f };
	v0.Color = color;
	v0.TextureCoords = { 0.0f, 0.0f };
	v0.TextureID = texID;

	Vertex v1;
	v1.Position = { x + size, y, 0.0f };
	v1.Color = color;
	v1.TextureCoords = { 1.0f, 0.0f };
	v1.TextureID = texID;

	Vertex v2;
	v2.Position = { x + size, y + size, 0.0f };
	v2.Color = color;
	v2.TextureCoords = { 1.0f, 1.0f };
	v2.TextureID = texID;

	Vertex v3;
	v3.Position = { x, y + size, 0.0f };
	v3.Color = color;
	v3.TextureCoords = { 0.0f, 1.0f };
	v3.TextureID = texID;

	return { v0, v1, v2, v3 };
}

namespace testSpace
{
	TestTexture2D::TestTexture2D()
		: translationB(0, 50, 0), proj(glm::ortho(-100.0f, 100.0f, -75.0f, 75.0f, -1.0f, 1.0f))
	{
		std::cout << "init start" << "\n" << std::endl;

		unsigned int indices[] =
		{
			0, 1, 2, 2, 3, 0,
			4, 5, 6, 6, 7, 4
		};

		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		va = std::make_unique<VertexArray>();
		va->Gen();

		ib = std::make_unique<IndexBuffer>(sizeof(indices));
		ib->Gen(indices);

		vb = std::make_unique<VertexBuffer>(sizeof(Vertex) * 1000);
		vb->Gen(nullptr);

		VertexBufferLayout layout;

		layout.Push<float>(3); // Position
		layout.Push<float>(4); // Colour
		layout.Push<float>(2); // TexCoords
		layout.Push<float>(1); // TexIndex

		va->AddBuffer(*vb, layout);

		std::cout << "size of vertex" << sizeof(Vertex) << "\n" << std::endl;
		std::cout << "offset of color" << offsetof(Vertex, Color) << "\n" << std::endl;

		shader = std::make_unique<Shader>("Res/Shaders/Basic.shader");
		shader->CreateShader();
		shader->Bind();

		int samplers[] = { 0, 1 };
		shader->SetUniform1iv("u_Textures", 2, samplers);

		texture = std::make_unique<Texture>("Res/Textures/namjas.JPG");
		texture->Gen();
		texture->Bind();

		texture1 = texture->GetBufferID();

		texture->Load("Res/Textures/shronk.jpg");
		texture2 = texture->GetBufferID();

		va->Unbind();
		shader->UnBind();
		vb->Unbind();
		ib->Unbind();

		std::cout << "init end" << "\n" << std::endl;
	}

	TestTexture2D::~TestTexture2D()
	{

	}

	void TestTexture2D::OnUpdate(float deltaTime)
	{
		translationB.x = ((cos((deltaTime) * 1)) * 10);
		translationB.y = ((sin((deltaTime) * 1)) * 10);

		camPos += glm::vec2(inputVector.x * 100.0f * deltaTime, inputVector.y * 100.0f * deltaTime);
		namPos += glm::vec2(namVector.x * 100.0f * deltaTime, namVector.y * 100.0f * deltaTime);

		inputVector.x = InputManager.GetActionStrength("right") - InputManager.GetActionStrength("left");
		inputVector.y = InputManager.GetActionStrength("up") - InputManager.GetActionStrength("down");

		namVector.x = InputManager.GetActionStrength("arrowRight") - InputManager.GetActionStrength("arrowLeft");
		namVector.y = InputManager.GetActionStrength("arrowUp") - InputManager.GetActionStrength("arrowDown");

		this->deltaTime = deltaTime;
	}

	void TestTexture2D::OnRender()
	{
		GLCall(glClearColor(0.05f, 0.05f, 0.05f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT));

		auto q0 = CreateQuad(-upVertex, -vertex, 0, { 0.18f, 0.6f, 0.96f, 1.0f });
		auto q1 = CreateQuad(vertex, -vertex, 1, { 1.0f, 0.93f, 0.24f, 1.0f });

		Vertex vertices[8];
		memcpy(vertices, q0.data(), q0.size() * sizeof(Vertex));
		memcpy(vertices + q0.size(), q1.data(), q1.size() * sizeof(Vertex));

		Renderer renderer;

		view = glm::translate(glm::mat4(1.0f), glm::vec3(-camPos.x, -camPos.y, 0)); // Camera translation.
		model = glm::translate(glm::mat4(1.0f), glm::vec3(namPos.x, namPos.y, 0)); // Model translation.

		mvp = proj * view * model;

		shader->Bind();
		shader->SetUniform4f("u_Color", glm::vec4(red, green, blue, alpha));

		shader->BindTexture(0, texture1);
		shader->BindTexture(1, texture2);

		vb->Bind();
		vb->ModData(vertices);

		shader->SetUniformMat4f("u_MVP", mvp);
		renderer.Draw(*va, *ib, *shader);

		//model = glm::translate(glm::mat4(1.0f), translationB); // Model translation.
		//mvp = proj * view * model;
	}

	void TestTexture2D::OnImGuiRender()
	{
		ImGui::Text("FPS %.1f FPS", double(ImGui::GetIO().Framerate));

		ImGui::Text("Control RGB values of shader");
		ImGui::SliderFloat("G:", &green, 0.0f, 1.0f, "%.1f");
		ImGui::SliderFloat("B:", &blue, 0.0f, 1.0f, "%.1f");
		ImGui::SliderFloat("A:", &alpha, 0.0f, 1.0f, "%.1f");
	}

	void TestTexture2D::OnHandleInput(GLFWwindow* window, int key, int scanCode, int action, int mods)
	{
		InputManager.actionMap[key].SetStrength(action);
	}

	float TestTexture2D::Clamp(float var, float min, float max)
	{
		const float tempVar = (var > min) ? var : min;
		return (tempVar < max) ? tempVar : max;
	}
}