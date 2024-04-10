#include "testtexture2d.h"
#include "Headers/renderer.h"

#include "Headers/shader.h"
#include "glm/gtc/matrix_transform.hpp"
#include <array>

glm::vec2 inputVector, namVector, namPos;
bool horizontal, vertical;

int index;
float vertex = 50.0f;
float upVertex = vertex * 3;
float sprintSpeed;

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

static Vertex* CreateQuad(Vertex* target, float x, float y, float texID, Vec4 color)
{
	float size = 100.0f;

	target->Position = { x, y, 0.0f };
	target->Color = color;
	target->TextureCoords = { 0.0f, 0.0f };
	target->TextureID = texID;
	target++;

	target->Position = { x + size, y, 0.0f };
	target->Color = color;
	target->TextureCoords = { 1.0f, 0.0f };
	target->TextureID = texID;
	target++;

	target->Position = { x + size, y + size, 0.0f };
	target->Color = color;
	target->TextureCoords = { 1.0f, 1.0f };
	target->TextureID = texID;
	target++;

	target->Position = { x, y + size, 0.0f };
	target->Color = color;
	target->TextureCoords = { 0.0f, 1.0f };
	target->TextureID = texID;
	target++;

	return target;
}

namespace testSpace
{
	// Scene initializer.
	TestTexture2D::TestTexture2D()
		: translationB(0, 50, 0), proj(glm::ortho(-100.0f, 100.0f, -75.0f, 75.0f, -1.0f, 1.0f))
	{

		const size_t MaxQuadCount = 1000, MaxVertexCount = MaxQuadCount * 4, MaxIndexCount = MaxQuadCount * 6;

		uint32_t indices[MaxIndexCount];
		uint32_t offset = 0;

		for (size_t i = 0; i < MaxIndexCount; i += 6) // Creates as many vertices as max.
		{
			indices[i + 0] = 0 + offset;
			indices[i + 1] = 1 + offset;
			indices[i + 2] = 2 + offset;

			indices[i + 3] = 2 + offset;
			indices[i + 4] = 3 + offset;
			indices[i + 5] = 0 + offset;

			offset += 4;
		}

		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		va = std::make_unique<VertexArray>();
		va->Gen();

		ib = std::make_unique<IndexBuffer>(sizeof(indices));
		ib->Gen(indices);

		vb = std::make_unique<VertexBuffer>(MaxVertexCount * sizeof(Vertex));
		vb->Gen(nullptr);

		VertexBufferLayout layout;

		layout.Push<float>(3); // Position
		layout.Push<float>(4); // Colour
		layout.Push<float>(2); // TexCoords
		layout.Push<float>(1); // TexIndex

		va->AddBuffer(*vb, layout);

		shader = std::make_unique<Shader>("Res/Shaders/Basic.shader");
		shader->CreateShader();
		shader->Bind();

		int samplers[] = { 0, 1, 2 }; // How many texture slots.
		shader->SetUniform1iv("u_Textures", sizeof(samplers), samplers); // Sets the shader texture slots to samplers.

		texture = std::make_unique<Texture>("Res/Textures/namjas.JPG");
		texture->Gen();
		texture->Bind();

		namjasTexture = texture->GetBufferID();
		shrekTexture = texture->Load("Res/Textures/shronk.jpg");
		monkeyTexture = texture->Load("Res/Textures/B happy.png");

		shader->BindTexture(0, namjasTexture);
		shader->BindTexture(1, shrekTexture);
		shader->BindTexture(2, monkeyTexture);

		va->Unbind();
		shader->UnBind();
		vb->Unbind();
		ib->Unbind();
	}

	TestTexture2D::~TestTexture2D()
	{

	}

	// Frame-by-frame scene logic.
	void TestTexture2D::OnUpdate(float deltaTime)
	{
		translationB.x = ((cos((deltaTime) * 1)) * 10);
		translationB.y = ((sin((deltaTime) * 1)) * 10);

		camPos += glm::vec2(inputVector.x * (100.0f + sprintSpeed) * deltaTime, inputVector.y * (100.0f + sprintSpeed) * deltaTime);
		namPos += glm::vec2(namVector.x * 100.0f * deltaTime, namVector.y * 100.0f * deltaTime);

		inputVector.x = InputManager.GetActionStrength("right") - InputManager.GetActionStrength("left");
		inputVector.y = InputManager.GetActionStrength("up") - InputManager.GetActionStrength("down");

		namVector.x = InputManager.GetActionStrength("arrowRight") - InputManager.GetActionStrength("arrowLeft");
		namVector.y = InputManager.GetActionStrength("arrowUp") - InputManager.GetActionStrength("arrowDown");

		sprintSpeed = (InputManager.GetActionStrength("sprint")) ? 100 : 0;

		this->deltaTime = deltaTime;
	}

	// Frame-by-frame rendering logic.
	void TestTexture2D::OnRender()
	{
		GLCall(glClearColor(0.05f, 0.05f, 0.05f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT));

		std::array<Vertex, 1000> vertices;
		Vertex* buffer = vertices.data();

		for (int y = 0; y < 5; y++)
		{
			for (int x = 0; x < 5; x++)
			{
				buffer = CreateQuad(buffer, x * 100, y * 100, (x + y) % 2, {1.0f, 0.93f, 0.24f, 1.0f}); // Creates a grid of quads.
			}
		}

		buffer = CreateQuad(buffer, -upVertex - 150, -vertex, 0, { 0.18f, 0.6f, 0.96f, 1.0f }); // Creates single quad.
		buffer = CreateQuad(buffer, -upVertex, -vertex, 2, { 1.0f, 1.0f, 1.0f, 1.0f }); // Creates single quad.

		Renderer renderer;

		view = glm::translate(glm::mat4(1.0f), glm::vec3(-camPos.x, -camPos.y, 0)); // Camera translation.
		model = glm::translate(glm::mat4(1.0f), glm::vec3(namPos.x, namPos.y, 0)); // Model translation.

		mvp = proj * view * model;

		shader->Bind();
		shader->SetUniform4f("u_Color", glm::vec4(red, green, blue, alpha));

		shader->BindTexture(0, index);

		vb->Bind();
		vb->ModifyData(vertices.size() * sizeof(Vertex), vertices.data());

		shader->SetUniformMat4f("u_MVP", mvp);
		renderer.Draw(*va, *ib, *shader);
	}

	// Frame-by-frame GUI logic.
	void TestTexture2D::OnImGuiRender()
	{
		ImGui::Text("FPS %.1f FPS", double(ImGui::GetIO().Framerate));

		ImGui::Text("Control RGB values of shader");
		ImGui::SliderFloat("R:", &red, 0.0f, 1.0f, "%.1f");
		ImGui::SliderFloat("G:", &green, 0.0f, 1.0f, "%.1f");
		ImGui::SliderFloat("B:", &blue, 0.0f, 1.0f, "%.1f");
		ImGui::SliderFloat("A:", &alpha, 0.0f, 1.0f, "%.1f");

		ImGui::SliderInt("Texture Index:", &index, 0, 3);
	}

	// Keyboard input handler.
	void TestTexture2D::OnHandleInput(GLFWwindow* window, int key, int scanCode, int action, int mods)
	{
		InputManager.actionMap[key].SetStrength(action);
	}

	// Simple float clamping function.
	float TestTexture2D::Clamp(float var, float min, float max)
	{
		const float tempVar = (var > min) ? var : min;
		return (tempVar < max) ? tempVar : max;
	}
}