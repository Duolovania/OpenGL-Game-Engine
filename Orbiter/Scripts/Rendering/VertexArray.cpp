#include "Rendering/vertexarray.h"
#include "Core/renderer.h"

VertexArray::VertexArray()
{

}

VertexArray::~VertexArray()
{
	GLCall(glDeleteVertexArrays(1, &rendererID));
}

// Generates vertex array data.
void VertexArray::Gen()
{
	GLCall(glGenVertexArrays(1, &rendererID));
}

// Selects this as current vertex array.
void VertexArray::Bind() const
{
	GLCall(glBindVertexArray(rendererID));
}

// Deselects vertex array.
void VertexArray::Unbind() const
{
	GLCall(glBindVertexArray(0));
}

// Enables and defines the vertex attrib array using vertex buffer and vertex buffer layout.
void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout, unsigned int offset)
{
	Bind();
	vb.Bind();
	const auto& elements = layout.GetElements(); 
	for (unsigned int i = 0; i < elements.size(); i++)
	{
		const auto& element = elements[i];
		GLCall(glEnableVertexAttribArray(i));
		GLCall(glVertexAttribPointer(i, element.count, element.type, element.normalised, layout.GetStride(), (const void*) offset));

		offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
	}
}