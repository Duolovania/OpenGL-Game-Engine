#pragma once
#include "orbiter.h"

class Editor : public RenderingLayer
{
	public:
		void Init(GLFWwindow* window) override;

		bool OnUpdate(float deltaTime) override;
		void Close() override;
		void CleanUp() override;
	private:
		void StylesConfig();

		void Viewport();
		void Hierarchy();
		void Inspector();
		void ContentBrowser();
		void MenuBar();

		void AudioManagerComponent();

		void CreateTransformColumn(const std::array<std::string, 3>& colNames, std::array<Vector3, 3> values);

		bool showStats = false, wireframeMode = false, showFPS = false, applicationRunning = true;
		int actionIndex = 0, keyBindIndex = 0;

		ImVec2 viewportSize;
		glm::vec2 inputVector;
		Renderer renderer;
		Camera2D camera2D;

		std::unique_ptr<VertexBuffer> frameBufferVB;
		std::unique_ptr<VertexArray> frameBufferVA;
};