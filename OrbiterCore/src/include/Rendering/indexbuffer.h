#pragma once

class IndexBuffer
{
	public:
		IndexBuffer(unsigned int count);
		~IndexBuffer();

		void Gen(const unsigned int* data);

		void Bind() const;
		void Unbind() const;

		inline unsigned int GetCount() const { return count; }
	private:
		unsigned int rendererID, count;
};