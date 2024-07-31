#include "testclearcolour.h"
#include "Headers/Core/renderer.h"

float tria[] = {
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.0f,  0.5f, 0.0f
};

unsigned int VAO, VBO;

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

		GLCall(glGenBuffers(1, &VBO));
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
		GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(tria), tria, GL_STATIC_DRAW));

		glGenVertexArrays(1, &VAO);

		// 1. bind Vertex Array Object
		glBindVertexArray(VAO);
		// 2. copy our vertices array in a buffer for OpenGL to use
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(tria), tria, GL_STATIC_DRAW);
		// 3. then set our vertex attributes pointers
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}

	void TestClearColour::OnImGuiRender()
	{
		ImGui::ColorEdit4("Clear Color", clearColour);
	}
}