#pragma once
#include <iostream>

class Texture
{
	public:
		Texture(const std::string& path);
		Texture();

		~Texture();

		void Bind(unsigned int slot = 0) const;
		void UnBind() const;

		void Gen(bool isPixelated = false);

		unsigned int Load(const std::string& path, bool isPixelated = false);
		unsigned int GetBufferID();
		void SetBuffer(unsigned int newBuffer);

		inline int GetWidth() { return w; }
		inline int GetHeight() { return h; }

		std::string GetImagePath() const;
	private:
		std::string filePath;
		unsigned int bufferID;
		unsigned char* localBuffer;
		int w, h, bpp; // Width, Height and Bits per Pixel
};