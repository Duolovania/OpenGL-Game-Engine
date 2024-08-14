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
void Texture::Gen()
{
	GLCall(glGenTextures(1, &bufferID));
	GLCall(glBindTexture(GL_TEXTURE_2D, bufferID));

	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR)); // GL_LINEAR = smooth. GL_NEAREST = pixelated.
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE)); // GL_CLAMP_TO_EDGE = extends image to size. GL_REPEAT = repeats the image to fit the size.
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, localBuffer));

	GLCall(glGenerateMipmap(GL_TEXTURE_2D));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0)); // Unbinds the texture.

	if (localBuffer)
		stbi_image_free(localBuffer);
}

// Generates texture data and returns the buffer ID (returns the new texture data through the ID).
unsigned int Texture::Load(const std::string& path)
{
	stbi_set_flip_vertically_on_load(1);
	localBuffer = stbi_load(path.c_str(), &w, &h, &bpp, 4);

	Gen();
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

