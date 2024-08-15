#include "testtexture2d.h"

#include "gtc/matrix_transform.hpp"
#include "Core/application.h"
#include "Rendering/shader.h"
#include <string>

#include <array>
#include "imgui_stdlib.h"
#include "Rendering/framebuffer.h"

namespace testSpace
{
	// Scene initializer.
	TestTexture2D::TestTexture2D()
	{

	}

	TestTexture2D::~TestTexture2D()
	{

	}

	// Frame-by-frame scene logic.
	void TestTexture2D::OnUpdate(float deltaTime)
	{
		this->deltaTime = deltaTime;
	}

	// Frame-by-frame rendering logic.
	void TestTexture2D::OnRender()
	{

	}

	// Frame-by-frame GUI logic.
	void TestTexture2D::OnImGuiRender()
	{
		
	}

	// Simple float clamping function.
	float TestTexture2D::Clamp(float var, float min, float max)
	{
		const float tempVar = (var > min) ? var : min;
		return (tempVar < max) ? tempVar : max;
	}
}