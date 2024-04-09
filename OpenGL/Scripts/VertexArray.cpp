#include "Headers/VertexArray.h"
#include "Headers/Renderer.h"
#include "Headers/VertexBufferLayout.h"

VertexArray::VertexArray()
{

}

void VertexArray::Gen()
{
	GLCall(glGenVertexArrays(1, &rendererID));
}

VertexArray::~VertexArray()
{
	GLCall(glDeleteVertexArrays(1, &rendererID));
}

void VertexArray::Bind() const
{
	GLCall(glBindVertexArray(rendererID));
}

void VertexArray::Unbind() const
{
	GLCall(glBindVertexArray(0));
}

void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout, unsigned int offset)
{
	Bind();
	vb.Bind();
	const auto& elements = layout.GetElements(); 
	//unsigned int offset = 0;
	for (unsigned int i = 0; i < elements.size(); i++)
	{
		const auto& element = elements[i];
		std::cout << "index:" << i  << ", count: " << element.count << ", normalised:" << element.normalised << ", stride: " << layout.GetStride() << ", offset: " << offset << std::endl;
		GLCall(glEnableVertexAttribArray(i));
		GLCall(glVertexAttribPointer(i, element.count, element.type, element.normalised, layout.GetStride(), (const void*) offset));

		offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
	}
}