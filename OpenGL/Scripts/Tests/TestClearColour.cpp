#include "testclearcolour.h"
#include "Headers/Renderer.h"

namespace testSpace
{
	TestClearColour::TestClearColour()
		: clearColour{0.2f, 0.3f, 0.8f, 1.0f}
	{

	}

	TestClearColour::~TestClearColour()
	{

	}

	void TestClearColour::OnUpdate(float deltaTime)
	{

	}

	void TestClearColour::OnRender()
	{
		GLCall(glClearColor(clearColour[0], clearColour[1], clearColour[2], clearColour[3]));
		GLCall(glClear(GL_COLOR_BUFFER_BIT));
	}

	void TestClearColour::OnImGuiRender()
	{
		ImGui::ColorEdit4("Clear Color", clearColour);
	}
}