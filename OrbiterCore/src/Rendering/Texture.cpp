#include "stb_image.h"
#include "Rendering/texture.h"
#include "Core/renderer.h"

Texture::Texture()
	: bufferID(0)
{

}

Texture::Texture(const std::string& path)
	: bufferID(0), filePath{path}, localBuffer(nullptr), w(0), h(0), bpp(0)
{
	stbi_set_flip_vertically_on_load(1);
	localBuffer = stbi_load(path.c_str(), &w, &h, &bpp, 4);
}

Texture::~Texture()
{
	GLCall(glDeleteTextures(1, &bufferID));
}

// Generates texture data.
void Texture::Gen(bool isPixelated)
{
	GLCall(glGenTextures(1, &bufferID));
	GLCall(glBindTexture(GL_TEXTURE_2D, bufferID));

	if (isPixelated)
	{
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST)); // GL_NEAREST = pixelated.
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	}
	else
	{
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR)); // GL_LINEAR = smooth.
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	}

	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE)); // GL_CLAMP_TO_EDGE = extends image to size. GL_REPEAT = repeats the image to fit the size.
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, localBuffer));

	GLCall(glGenerateMipmap(GL_TEXTURE_2D));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0)); // Unbinds the texture.

	if (localBuffer)
		stbi_image_free(localBuffer);
}

LiteTexture Texture::GenBindlessTexture(const std::string& path, bool isPixelated)
{
	LiteTexture newTexture;
	localBuffer = stbi_load(path.c_str(), &w, &h, &bpp, 4);

	Gen(isPixelated);
	newTexture.m_imagePath = path;

	newTexture.textureHandle = glGetTextureHandleARB(bufferID);
	GLCall(glMakeTextureHandleResidentARB(newTexture.textureHandle));

	newTexture.textureBuffer = bufferID;
	return newTexture;
}

unsigned int Texture::Load(const std::string& path, bool isPixelated)
{
	localBuffer = stbi_load(path.c_str(), &w, &h, &bpp, 4);

	Gen(isPixelated);
	return bufferID;
}

// Selects this as the current texture.
void Texture::Bind(unsigned int slot) const
{
	GLCall(glActiveTexture(GL_TEXTURE0 + slot));
	GLCall(glBindTexture(GL_TEXTURE_2D, bufferID));
}

// Resets the selection.
void Texture::UnBind() const
{
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}

// Returns the bufferID.
unsigned int Texture::GetBufferID()
{
	return bufferID;
}

void Texture::SetBuffer(unsigned int newBuffer)
{
	bufferID = newBuffer;
}

std::string Texture::GetImagePath() const
{
	return filePath;
}

