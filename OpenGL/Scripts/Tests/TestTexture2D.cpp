#include "testtexture2d.h"

#include "gtc/matrix_transform.hpp"
#include "Core/application.h"
#include "Rendering/shader.h"
#include <string>

#include <array>
#include "imgui_stdlib.h"
#include "Rendering/framebuffer.h"

glm::vec2 inputVector, namVector, namPos;
std::string cameraName = "Camera";
int selectedObject = -2;
float sprintSpeed;

namespace testSpace
{
	// Scene initializer.
	TestTexture2D::TestTexture2D()
	{
		renderer.Init();
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
	void TestTexture2D::OnRender(glm::mat4 proj)
	{
		GLCall(glClearColor(0.05f, 0.05f, 0.05f, 1.0f));
		renderer.Clear();

		view = glm::translate(glm::mat4(1.0f), glm::vec3(-camPos.x, -camPos.y, 0)); // Camera translation.
		model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)); // Model translation.

		mvp = proj * view * model;

		renderer.Draw(mvp, camPos);
	}

	// Frame-by-frame GUI logic.
	void TestTexture2D::OnImGuiRender()
	{
		ImGui::Text("Textures Loaded: %.0f", double(renderer.texturesLoaded));
		ImGui::Text("New Textures Created: %.0f", double(renderer.newTextures));

		ImGui::Begin("Inspector");

		if (selectedObject > -1)
		{
			ImGui::Text("Object Name:");
			ImGui::SameLine();
			ImGui::InputText("##label0", &renderer.objectsToRender[selectedObject].objectName);

			if (ImGui::CollapsingHeader("Transform"))
			{
				ImGui::Text("Position:");
				ImGui::SameLine();
				ImGui::InputFloat2("##label1", (float*) &renderer.objectsToRender[selectedObject].transform.position, "%.f");

				ImGui::Text("Scale:");
				ImGui::SameLine();
				ImGui::InputFloat2("##label2", (float*) &renderer.objectsToRender[selectedObject].transform.scale.x, "%.f");

				ImGui::Text("Rotation:");
				ImGui::SameLine();
				ImGui::InputFloat("##label3", &renderer.objectsToRender[selectedObject].transform.rotation.z, 0.0f, 0.0f, "%.f");
			}

			if (ImGui::CollapsingHeader("Sprite Renderer"))
			{
				ImGui::Image((void*)renderer.objectsToRender[selectedObject].texture, ImVec2(200, 200), ImVec2(0, 1), ImVec2(1, 0));

				ImGui::Text("Colour:");
				ImGui::SameLine();
				ImGui::ColorEdit4("##label4", renderer.objectsToRender[selectedObject].color);

				ImGui::SameLine();
				if (ImGui::Button("Reset"))
				{
					renderer.objectsToRender[selectedObject].SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
				}
			}

		}
		else if (selectedObject == -1)
		{
			ImGui::Text("Object Name:");
			ImGui::SameLine();
			ImGui::InputText("##label0", &cameraName);

			if (ImGui::CollapsingHeader("Transform"))
			{
				ImGui::Text("Position:");
				ImGui::SameLine();
				ImGui::InputFloat2("##label1", (float*) &camPos, "%.f");
			}
		}

		ImGui::End();

		ImGui::Begin("Hierarchy");

		if (ImGui::Button((cameraName).c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 0)))
		{
			selectedObject = -1;
		}

		for (int i = 0; i < renderer.objectsToRender.size(); i++)
		{
			if (ImGui::Button((renderer.objectsToRender[i].objectName).c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 0)))
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