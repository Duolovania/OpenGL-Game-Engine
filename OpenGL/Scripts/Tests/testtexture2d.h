#pragma once
#include "test.h"
#include "Headers/texture.h"
#include "Headers/VertexBufferLayout.h"
#include "Headers/VertexBuffer.h"
#include "Headers/input.h"

#include <memory>
#include <algorithm>

namespace testSpace
{
	class TestTexture2D : public Test
	{
		public:
			TestTexture2D();
			~TestTexture2D();

			void OnUpdate(float deltaTime) override;
			void OnImGuiRender() override;
			void OnRender() override;
			void OnHandleInput(GLFWwindow* window, int key, int scanCode, int action, int mods) override;
		private:
			glm::vec3 translationB;
			glm::vec2 camPos;

			std::unique_ptr<VertexArray> va;
			std::unique_ptr<IndexBuffer> ib;
			std::unique_ptr<VertexBuffer> vb;

			std::unique_ptr<Texture> texture;
			std::unique_ptr<Shader> shader;

			Input InputManager;
			
			glm::mat4 proj, view, model, mvp;  // Proj = Orthrographic projection.
			
			float deltaTime;
			float red = 1.0f, green = 1.0f, blue = 1.0f, alpha = 1.0f, increment = 1.0f;

			float Clamp(float var, float min, float max);
			unsigned int namjasTexture, shrekTexture, monkeyTexture;
	};
}
