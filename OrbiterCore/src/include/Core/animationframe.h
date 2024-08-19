#pragma once
#include <vector>
#include "Rendering/texture.h"

class AnimationFrame
{
	public:	
		AnimationFrame(const std::string& filePath);

		unsigned int GetTextureBuffer() const;
	private:
		std::shared_ptr<Texture> m_texture;
		std::string m_imageFilePath;
		unsigned int m_textureIndex;

		void GenerateTextures();
};