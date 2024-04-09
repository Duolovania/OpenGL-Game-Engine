#pragma once

class IndexBuffer
{
	private:
		unsigned int rendererID, count;
	public:
		IndexBuffer(unsigned int count);
		~IndexBuffer();

		void Gen(const unsigned int* data);

		void Bind() const;
		void Unbind() const;

		inline unsigned int GetCount() const { return count; }
};