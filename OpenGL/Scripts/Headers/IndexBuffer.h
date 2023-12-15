#pragma once

class IndexBuffer
{
	private:
		unsigned int rendererID, count;
	public:
		IndexBuffer(const unsigned int* data, unsigned int count);
		~IndexBuffer();

		void Gen(const unsigned int* data, unsigned int count);

		void Bind() const;
		void Unbind() const;

		inline unsigned int GetCount() const { return count; }
};