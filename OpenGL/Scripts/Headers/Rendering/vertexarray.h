#pragma once
#include "vertexbuffer.h"
#include "vertexbufferlayout.h"

class VertexArray
{
	private:
		unsigned int rendererID;
	public:
		VertexArray();
		~VertexArray();
	
		void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout, unsigned int offset = 0);

		void Gen();
		void Bind() const;
		void Unbind() const;
};