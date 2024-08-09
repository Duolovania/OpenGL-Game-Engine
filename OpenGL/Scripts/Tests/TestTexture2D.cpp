#include "testtexture2d.h"

#include "gtc/matrix_transform.hpp"
#include "Core/application.h"
#include "Rendering/shader.h"
#include <string>

#include <array>
#include "imgui/imgui_stdlib.h"

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
		proj = glm::ortho(((float) 1920 / (float) 1080) * -100, ((float)1920 / (float)1080) * 100, -100.0f, 100.0f, -1.0f, 1.0f);
	}

	TestTexture2D::~TestTexture2D()
	{

	}

	// Frame-by-frame scene logic.
	void TestTexture2D::OnUpdate(float deltaTime)
	{
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
		renderer.Clear();

		view = glm::translate(glm::mat4(1.0f), glm::vec3(-camPos.x, -camPos.y, 0)); // Camera translation.
		model = glm::translate(glm::mat4(1.0f), glm::vec3(namPos.x, namPos.y, 0)); // Model translation.

		mvp = proj * view * model;

		renderer.Draw(mvp, camPos);
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

		if (selectedObject > -1)
		{
			ImGui::Begin("Inspector");

			ImGui::InputText("Object Name:", &renderer.objectsToRender[selectedObject].objectName); // new change.

			if (ImGui::CollapsingHeader("Transform"))
			{
				ImGui::InputFloat("PX:", &renderer.objectsToRender[selectedObject].transform.position.x, 0.0f, 0.0f, "%.f");
				ImGui::SameLine();

				ImGui::InputFloat("PY:", &renderer.objectsToRender[selectedObject].transform.position.y, 0.0f, 0.0f, "%.f");

				ImGui::InputFloat("SX:", &renderer.objectsToRender[selectedObject].transform.scale.x, 0.0f, 0.0f, "%.f");
				ImGui::SameLine();

				ImGui::InputFloat("SY:", &renderer.objectsToRender[selectedObject].transform.scale.y, 0.0f, 0.0f, "%.f");

				ImGui::InputFloat("RZ:", &renderer.objectsToRender[selectedObject].transform.rotation.z, 0.0f, 0.0f, "%.f");
			}

			if (ImGui::CollapsingHeader("Sprite Renderer"))
			{
				ImGui::Image((void*)renderer.objectsToRender[selectedObject].texture, ImVec2(200, 200), ImVec2(0, 1), ImVec2(1, 0));

				ImGui::Text("Colour Control:");
				ImGui::SliderFloat("R:", &renderer.objectsToRender[selectedObject].color.x, 0.0f, 1.0f, "%.1f");
				ImGui::SameLine();

				ImGui::SliderFloat("G:", &renderer.objectsToRender[selectedObject].color.y, 0.0f, 1.0f, "%.1f");
				ImGui::SameLine();

				ImGui::SliderFloat("B:", &renderer.objectsToRender[selectedObject].color.z, 0.0f, 1.0f, "%.1f");
				ImGui::SameLine();

				ImGui::SliderFloat("A:", &renderer.objectsToRender[selectedObject].color.w, 0.0f, 1.0f, "%.1f");
			}

			ImGui::End();
		}

		ImGui::Begin("Hierarchy");

		for (int i = 0; i < renderer.objectsToRender.size(); i++)
		{
			if (ImGui::Button((renderer.objectsToRender[i].objectName).c_str()))
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