#include "testtexture2d.h"

#include "gtc/matrix_transform.hpp"
#include "Headers/application.h"
#include "Headers/shader.h"

#include <array>

glm::vec2 inputVector, namVector, namPos;

int index = 2;
float vertex = 50.0f;
float upVertex = vertex * 3;
float sprintSpeed;

namespace testSpace
{
	// Scene initializer.
	TestTexture2D::TestTexture2D()
		: translationB(0, 50, 0), proj(glm::ortho(-100.0f, 100.0f, -75.0f, 75.0f, -1.0f, 1.0f))
	{
		renderer.Init();
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

		inputVector.x = Core.InputManager.GetActionStrength("right") - Core.InputManager.GetActionStrength("left");
		inputVector.y = Core.InputManager.GetActionStrength("up") - Core.InputManager.GetActionStrength("down");

		namVector.x = Core.InputManager.GetActionStrength("arrowRight") - Core.InputManager.GetActionStrength("arrowLeft");
		namVector.y = Core.InputManager.GetActionStrength("arrowUp") - Core.InputManager.GetActionStrength("arrowDown");

		sprintSpeed = Core.InputManager.GetActionStrength("sprint") * 150;

		this->deltaTime = deltaTime;
	}

	// Frame-by-frame rendering logic.
	void TestTexture2D::OnRender()
	{
		GLCall(glClearColor(0.05f, 0.05f, 0.05f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT));

		renderer.ClearVertices();

		// Creates a grid of quads.
		for (int y = 0; y < 5; y++)
		{
			for (int x = 0; x < 5; x++)
			{
				renderer.CreateQuad(x * 100, y * 100, (x + y) % 2, { 1.0f, 0.93f, 0.24f, 1.0f });
			}
		}

		renderer.CreateQuad(-upVertex - 150, -vertex, 0, { 0.18f, 0.6f, 0.96f, 1.0f }); // Creates single quad.
		renderer.CreateQuad(-upVertex, -vertex, 2, { 1.0f, 1.0f, 1.0f, 1.0f }); // Creates single quad.

		view = glm::translate(glm::mat4(1.0f), glm::vec3(-camPos.x, -camPos.y, 0)); // Camera translation.
		model = glm::translate(glm::mat4(1.0f), glm::vec3(namPos.x, namPos.y, 0)); // Model translation.

		mvp = proj * view * model;

		renderer.ChangeTexture(index);

		renderer.Draw(mvp, glm::vec4(red, green, blue, alpha));
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

		ImGui::SliderInt("Texture Index:", &index, 2, 3);
	}

	// Simple float clamping function.
	float TestTexture2D::Clamp(float var, float min, float max)
	{
		const float tempVar = (var > min) ? var : min;
		return (tempVar < max) ? tempVar : max;
	}
}