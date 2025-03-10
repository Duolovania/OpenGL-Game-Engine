#pragma once
#include "orbiter.h"

class Editor : public RenderingLayer
{
	public:
		void Init(GLFWwindow* window) override;

		bool OnUpdate(float deltaTime, float time) override;
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
		void SearchMainCamera();

		void CreateTransformColumn(const std::array<std::string, 3>& colNames, std::array<Vector3, 3> values);

		bool showStats = false, wireframeMode = false, showFPS = false, applicationRunning = true, savedChanges = true, isPlaying = false;
		int actionIndex = 0, keyBindIndex = 0;

		ImVec2 viewportSize;
		glm::vec2 inputVector;
		Renderer renderer;

		std::shared_ptr<Camera> camera;
		std::shared_ptr<GameObject> cameraObj;

		Scene currentScene;

		std::unique_ptr<VertexBuffer> frameBufferVB;
		std::unique_ptr<VertexArray> frameBufferVA;

		ImFont* pixelFont;
};