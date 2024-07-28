#pragma once
#include "test.h"
#include "Rendering/texture.h"
#include "Core/renderer.h"

#include "Rendering/vertexbufferlayout.h"
#include "Rendering/vertexbuffer.h"
#include "Core/input.h"
#include "Core/animationplayer.h"

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

			Renderer renderer;
			AnimationPlayer playerAnimator;
			
			glm::mat4 proj, view, model, mvp;  // Proj = Orthrographic projection.
			
			float deltaTime;
			float red = 1.0f, green = 1.0f, blue = 1.0f, alpha = 1.0f, imageScale = 1.0f;

			float Clamp(float var, float min, float max);
	};
}
