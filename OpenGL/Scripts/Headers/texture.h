#pragma once
#include "vector2.h"
#include "Renderer.h"

class Texture
{
	private:
		std::string filePath;
		unsigned int bufferID;
		unsigned char* localBuffer;
		int w, h, bpp; // Width, Height and Bits per Pixel

	public:
		Texture(const std::string& path);
		~Texture();

		void Bind(unsigned int slot = 0) const;
		void UnBind() const;

		void Gen();

		Vector2 position;
		inline int GetWidth() { return w; }
		inline int GetHeight() { return h; }
};