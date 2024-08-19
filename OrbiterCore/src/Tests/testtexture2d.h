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

			Renderer renderer;
			AnimationPlayer playerAnimator;
			
			float deltaTime;
			float Clamp(float var, float min, float max);
	};
}
