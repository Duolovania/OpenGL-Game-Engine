#pragma once
#include <iostream>
#include "glad/glad.h"

struct LiteTexture
{
	std::string m_imagePath;
	GLuint64 textureHandle = 0;
	unsigned int textureBuffer = 0;
};

class Texture
{
	public:
		Texture(const std::string& path);
		Texture();

		~Texture();

		void Bind(unsigned int slot = 0) const;
		void UnBind() const;

		void Gen(bool isPixelated = false);
		LiteTexture GenBindlessTexture(const std::string& path, bool isPixelated = false);

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
		int w, h, bpp; // Width, Height and Bits per Pixel.
};