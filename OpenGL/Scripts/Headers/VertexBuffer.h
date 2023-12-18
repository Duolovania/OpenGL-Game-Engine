#pragma once

class VertexBuffer
{
	private:
		unsigned int rendererID;
		unsigned int size;
	public:
		VertexBuffer(const void* data, unsigned int size);
		~VertexBuffer();

		void Bind() const;
		void Unbind() const;
		void ModData(const void* data);

		void Gen(const void* data);
};