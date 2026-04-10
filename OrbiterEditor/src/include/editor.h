#pragma once
#include "orbiter.h"

class Editor : public RenderingLayer
{
	public:
		void Init(GLFWwindow* window) override;

		bool OnUpdate(float deltaTime, float time) override;
		void Close() override;
	private:
		void StylesConfig();

		void Viewport();
		void GameView();
		void Hierarchy();
		void Inspector();
		void ContentBrowser();
		void MenuBar();

		void AudioManagerComponent();
		void ScriptManagerComponent();
		void SearchMainCamera();

		void DebugWindow();
		void OptionalWindows();
		
		// Add tooltip for UI element above.
		void AddTooltip(const char* message) const;
		void CreateTransformColumn(const std::array<std::string, 3>& colNames, std::array<Vector3, 3> values);

		bool applicationRunning = true, savedChanges = true, isPlaying = false;
		int actionIndex = 0, keyBindIndex = 0;

		EditorSettings selectedEditorConfig;

		ImVec2 viewportSize;
		glm::vec2 inputVector;

		GameObject editorCameraObj, gameCameraObj, previewCameraObj;

		Scene currentScene;

		std::unique_ptr<VertexBuffer> frameBufferVB;
		std::unique_ptr<VertexArray> frameBufferVA;

		ImFont* pixelFont;
};