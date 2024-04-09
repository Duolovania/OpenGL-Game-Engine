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
			float red = 0.0f, green = 1.0f, blue = 1.0f, alpha = 1.0f, increment = 1.0f;
			glm::vec2 camPos;

			std::unique_ptr<VertexArray> va;
			std::unique_ptr<IndexBuffer> ib;

			std::unique_ptr<Texture> texture, shrekTexture;
			std::unique_ptr<Shader> shader;
			std::unique_ptr<VertexBuffer> vb;

			Input InputManager;

			// Proj = Orthrographic projection.
			glm::mat4 proj, view, model, mvp;
			
			float deltaTime;
			float Clamp(float var, float min, float max);

			unsigned int texture1, texture2;

			//float positions[] =
		//{
		//	-upVertex, -vertex, 0.0f,	0.18f, 0.6f, 0.96f, 1.0f,   0.0f, 0.0f,   0.0f, // 0
		//	-vertex, -vertex, 0.0f,     0.18f, 0.6f, 0.96f, 1.0f,   1.0f, 0.0f,   0.0f, // 1
		//	-vertex, vertex, 0.0f,      0.18f, 0.6f, 0.96f, 1.0f,   1.0f, 1.0f,   0.0f, // 2
		//	-upVertex, vertex, 0.0f,    0.18f, 0.6f, 0.96f, 1.0f,   0.0f, 1.0f,   0.0f, // 3

		//	vertex, -vertex, 0.0f,    1.0f, 0.93f, 0.24f, 1.0f,     0.0f, 0.0f,   1.0f, // 4
		//	upVertex, -vertex, 0.0f,  1.0f, 0.93f, 0.24f, 1.0f,     1.0f, 0.0f,   1.0f, // 5
		//	upVertex, vertex, 0.0f,   1.0f, 0.93f, 0.24f, 1.0f,     1.0f, 1.0f,   1.0f, // 6
		//	vertex, vertex, 0.0f,    1.0f, 0.93f, 0.24f, 1.0f,      0.0f, 1.0f,   1.0f  // 7
		//};
	};
}
