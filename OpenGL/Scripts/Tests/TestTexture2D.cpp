#include "testtexture2d.h"

#include "gtc/matrix_transform.hpp"
#include "Core/application.h"
#include "Rendering/shader.h"
#include <string>

#include <array>

glm::vec2 inputVector, namVector, namPos;
int selectedObject = -1;
float sprintSpeed;

namespace testSpace
{
	// Scene initializer.
	TestTexture2D::TestTexture2D()
		: proj(glm::ortho(-100.0f, 100.0f, -75.0f, 75.0f, -1.0f, 1.0f))
	{
		renderer.Init();
	}

	TestTexture2D::~TestTexture2D()
	{

	}

	// Frame-by-frame scene logic.
	void TestTexture2D::OnUpdate(float deltaTime)
	{
		camPos += glm::vec2(inputVector.x * (10.0f + sprintSpeed) * deltaTime, inputVector.y * (10.0f + sprintSpeed) * deltaTime);
		namPos += glm::vec2(namVector.x * 100.0f * deltaTime, namVector.y * 100.0f * deltaTime);

		inputVector.x = Core.InputManager.GetActionStrength("right") - Core.InputManager.GetActionStrength("left");
		inputVector.y = Core.InputManager.GetActionStrength("up") - Core.InputManager.GetActionStrength("down");

		namVector.x = Core.InputManager.GetActionStrength("arrowRight") - Core.InputManager.GetActionStrength("arrowLeft");
		namVector.y = Core.InputManager.GetActionStrength("arrowUp") - Core.InputManager.GetActionStrength("arrowDown");

		sprintSpeed = Core.InputManager.GetActionStrength("sprint") * 15;

		this->deltaTime = deltaTime;
	}

	// Frame-by-frame rendering logic.
	void TestTexture2D::OnRender()
	{
		GLCall(glClearColor(0.05f, 0.05f, 0.05f, 1.0f));
		renderer.Clear();

		view = glm::translate(glm::mat4(1.0f), glm::vec3(-camPos.x, -camPos.y, 0)); // Camera translation.
		model = glm::translate(glm::mat4(1.0f), glm::vec3(namPos.x, namPos.y, 0)); // Model translation.

		mvp = proj * view * model;

		renderer.Draw(mvp, camPos, glm::vec4(red, green, blue, alpha));
	}

	// Frame-by-frame GUI logic.
	void TestTexture2D::OnImGuiRender()
	{
		ImGui::Text("FPS: %.1f", double(ImGui::GetIO().Framerate));
		ImGui::Text("Textures Loaded: %.0f", double(renderer.texturesLoaded));
		ImGui::Text("New Textures Created: %.0f", double(renderer.newTextures));

		ImGui::Text("Position X: %.0f", double(camPos.x));
		ImGui::SameLine();
		ImGui::Text("Y: %.0f", double(camPos.y));


		ImGui::Text("Control RGB values of shader");
		ImGui::SliderFloat("R:", &red, 0.0f, 1.0f, "%.1f");
		ImGui::SameLine();

		ImGui::SliderFloat("G:", &green, 0.0f, 1.0f, "%.1f");
		ImGui::SameLine();

		ImGui::SliderFloat("B:", &blue, 0.0f, 1.0f, "%.1f");
		ImGui::SameLine();

		ImGui::SliderFloat("A:", &alpha, 0.0f, 1.0f, "%.1f");

		ImGui::Begin("Inspector");

		if (ImGui::ListBoxHeader("Transform"))
		{
			if (selectedObject > -1)
			{
				ImGui::InputFloat("PX:", &renderer.objectsToRender[selectedObject].transform.position.x, 0.0f, 0.0f, "%.f");
				ImGui::SameLine();
				
				ImGui::InputFloat("PY:", &renderer.objectsToRender[selectedObject].transform.position.y, 0.0f, 0.0f, "%.f");

				ImGui::InputFloat("SX:", &renderer.objectsToRender[selectedObject].transform.scale.x, 0.0f, 0.0f, "%.f");
				ImGui::SameLine();

				ImGui::InputFloat("SY:", &renderer.objectsToRender[selectedObject].transform.scale.y, 0.0f, 0.0f, "%.f");

				ImGui::InputFloat("RZ:", &renderer.objectsToRender[selectedObject].transform.rotation.z, 0.0f, 0.0f, "%.f");
			}

			ImGui::ListBoxFooter();
		}

		ImGui::End();

		ImGui::Begin("Hierarchy");

		for (int i = 0; i < renderer.objectsToRender.size(); i++)
		{
			if (ImGui::Button(std::to_string(i).c_str()))
			{
				selectedObject = i;
			}
		}

		ImGui::End();
	}

	// Simple float clamping function.
	float TestTexture2D::Clamp(float var, float min, float max)
	{
		const float tempVar = (var > min) ? var : min;
		return (tempVar < max) ? tempVar : max;
	}
}