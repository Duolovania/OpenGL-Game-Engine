#pragma once
#include <iostream>

class Texture
{
	public:
		Texture(const std::string& path);
		~Texture();

		void Bind(unsigned int slot = 0) const;
		void UnBind() const;

		void Gen();

		unsigned int Load(const std::string& path);
		unsigned int GetBufferID();

		inline int GetWidth() { return w; }
		inline int GetHeight() { return h; }
	private:
		std::string filePath;
		unsigned int bufferID;
		unsigned char* localBuffer;
		int w, h, bpp; // Width, Height and Bits per Pixel
};