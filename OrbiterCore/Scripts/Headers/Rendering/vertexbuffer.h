#pragma once

class VertexBuffer
{
	private:
		unsigned int rendererID;
		unsigned int size;
	public:
		VertexBuffer(unsigned int size);
		~VertexBuffer();

		void Bind() const;
		void Unbind() const;
		void ModifyData(unsigned int size, const void* data);
		void ModifyData(const void* data);

		void Gen(const void* data);
		void GenStatic(const void* data);
};