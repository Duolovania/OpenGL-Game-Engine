#pragma once
#include "test.h"
#include "Headers/texture.h"
#include "Headers/vertexbufferlayout.h"
#include "Headers/vertexbuffer.h"
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
		private:
			glm::vec2 camPos = glm::vec2(0, 0);
			glm::vec3 translationB;

			std::unique_ptr<VertexArray> va;
			std::unique_ptr<IndexBuffer> ib;
			std::unique_ptr<VertexBuffer> vb;

			std::unique_ptr<Texture> texture;
			std::unique_ptr<Shader> shader;
			
			glm::mat4 proj, view, model, mvp;  // Proj = Orthrographic projection.
			
			float deltaTime;
			float red = 1.0f, green = 1.0f, blue = 1.0f, alpha = 1.0f, increment = 1.0f;

			float Clamp(float var, float min, float max);
			unsigned int namjasTexture, shrekTexture, monkeyTexture;
	};
}
