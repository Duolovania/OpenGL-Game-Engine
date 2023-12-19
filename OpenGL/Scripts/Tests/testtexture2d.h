#pragma once
#include "test.h"
#include "Headers/texture.h"
#include "Headers/VertexBufferLayout.h"
#include "Headers/VertexBuffer.h"
#include <memory>

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
			glm::vec3 translationA, translationB;
			float red = 0.0f, green = 1.0f, blue = 1.0f, alpha = 1.0f, increment = 1.0f;
			float camX = 0.0, camY = 0.0;

			std::unique_ptr<VertexArray> va;
			std::unique_ptr<IndexBuffer> ib;

			std::unique_ptr<Texture> texture;
			std::unique_ptr<Shader> shader;
			std::unique_ptr<VertexBuffer> vb;

			// Proj = Orthrographic projection.
			glm::mat4 proj, view, model, mvp;

			float vertex = 50.0f;
	};
}
