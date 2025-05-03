#pragma once
#include <iostream>
#include "Core/renderer.h"

// Clears OpenGL errors.
void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

// Returns a more detailed error for OpenGL errors.
bool GLLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGL Error] (" << error << "): " << function << " " << file << ":" << line << std::endl; 
        return false;
    }

    return true;
}

Renderer::Renderer()
{
	
}

Renderer::~Renderer()
{
	m_va->Unbind();
	m_shader->UnBind();
	m_vb->Unbind();
	m_ib->Unbind();
}

// Generates essential rendering data. This must be called before draw().
void Renderer::Init()
{
	const size_t MaxQuadCount = 200, MaxVertexCount = MaxQuadCount * 4, MaxIndexCount = MaxQuadCount * 6;

	uint32_t offset = 0;
	uint32_t indices[MaxIndexCount];

	// Creates as many vertices as max.
	for (size_t i = 0; i < MaxIndexCount; i += 6) 
	{
		indices[i + 0] = 0 + offset;
		indices[i + 1] = 1 + offset;
		indices[i + 2] = 2 + offset;

		indices[i + 3] = 2 + offset;
		indices[i + 4] = 3 + offset;
		indices[i + 5] = 0 + offset;

		offset += 4;
	}

	GLCall(glEnable(GL_BLEND));
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

	// Creates and generates vertex array.
	m_va = std::make_unique<VertexArray>();
	m_va->Gen();

	// Creates and generates index buffer.
	m_ib = std::make_unique<IndexBuffer>(sizeof(indices));
	m_ib->Gen(indices);

	// Creates and generates vertex buffer.
	m_vb = std::make_unique<VertexBuffer>(MaxVertexCount * sizeof(Vertex));
	m_vb->Gen(nullptr);

	VertexBufferLayout layout;

	// Defines the layout of the vertex buffer.
	layout.Push<float>(3); // Position (takes 3 params).
	layout.Push<float>(4); // Colour (takes 4 params).
	layout.Push<float>(2); // TexCoords (takes 2 params).
	layout.Push<float>(1); // TexIndex (has 1 value).

	m_va->AddBuffer(*m_vb, layout); // Adds the vertex buffer to the vertex array.

	// Creates and generates the shader.
	m_shader = std::make_unique<Shader>("../OrbiterCore/Res/Shaders/Basic.shader");
	m_shader->CreateShader();
	m_shader->Bind();

	// Generates and caches the sample white square image as a texture.
	LiteTexture newTexture = m_text.GenBindlessTexture("../OrbiterCore/Res/Default Sprites/whitesqure.png");
	cachedTextures.push_back(newTexture);

	m_va->Unbind();
	m_shader->UnBind();
	m_vb->Unbind();
	m_ib->Unbind();
}

// Outputs the rendering data onto the viewport.
void Renderer::Draw(glm::mat4 projection, glm::mat4 view, glm::vec4 colourTint)
{
	buffer = vertices.data(); // Clears the buffer data.
	m_shader->Bind();

	// Loops through each object.
	for (int i = 0; i < objectsToRender.size(); i++)
	{
		// Checks if the object has a sprite renderer component.
		if (objectsToRender[i]->HasComponent("Sprite Renderer"))
		{
			std::shared_ptr<SpriteRenderer> spriteRenderer = objectsToRender[i]->GetComponent<SpriteRenderer>();

			// Checks if the sprite is inside the camera.
			if (spriteRenderer->CheckVisibility(glm::vec2(view[3].x, view[3].y)))
			{
				// Sets the transform matrix to the object's transform values.
				glm::mat4 transform =
					glm::translate(glm::mat4(1.0f), glm::vec3(objectsToRender[i]->transform.position.x, objectsToRender[i]->transform.position.y, 0.0f))
					* glm::rotate(glm::mat4(1.0f), glm::radians(-objectsToRender[i]->transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f))
					* glm::scale(glm::mat4(1.0f), glm::vec3(objectsToRender[i]->transform.scale.x, objectsToRender[i]->transform.scale.y, 1.0f));

				// Creates a new quad with the transform matrix and sprite renderer colour values.
				buffer = CreateQuad(buffer, transform, i, { spriteRenderer->color[0], spriteRenderer->color[1], spriteRenderer->color[2], spriteRenderer->color[3] });
			}
		}
	}

	m_vb->Bind();
	m_vb->ModifyData(vertices.size() * sizeof(Vertex), vertices.data());

	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)); // Model translation.
	glm::mat4 mvp = projection * view * model;

	m_shader->SetUniformMat4f("u_MVP", mvp); // Sets the model-view-projection.
	m_shader->SetUniform4f("u_Color", colourTint); // Tints the camera.

	//m_shader->SetUniform2f("lightPosition", glm::vec2(view[3].x, view[3].y));
	//m_shader->SetUniform2f("position", glm::vec2(view[3].x, view[3].y));

	m_va->Bind();
	m_ib->Bind();

	GLCall(glDrawElements(GL_TRIANGLES, m_ib->GetCount(), GL_UNSIGNED_INT, nullptr)); // Draws data.

	// Unbinds all rendering components.
	m_shader->UnBind();
	m_vb->Unbind();
	m_ib->Unbind();
	m_va->Unbind();
}

// Regenerates all objects. This should be called if multiple textures must be refreshed (e.g. image file has changed.)
void Renderer::RegenerateObjects()
{
	texturesLoaded = 0;
	newTextures = 0;

	m_shader->Bind();

	std::fill(samplers, samplers + (sizeof(samplers) / sizeof(samplers[0])), 0);

	// Prepares necessary amount of slots and binds each character texture to a slot.
	for (int i = 0; i < objectsToRender.size(); i++)
	{
		if (objectsToRender[i]->HasComponent("Sprite Renderer"))
		{
			std::shared_ptr<SpriteRenderer> spriteRenderer = objectsToRender[i]->GetComponent<SpriteRenderer>();
			spriteRenderer->cTexture = GetCachedBindlessTexture(spriteRenderer);
			samplers[i] = spriteRenderer->cTexture.textureHandle;

			texturesLoaded++;
		}
	}

	//m_shader->SetUniform3f("ambientLight", glm::vec3(0.05, 0.05, 0.05));
	m_shader->SetUniformHandlei64vARB("u_Textures", sizeof(samplers), samplers); // Sets the shader texture slots to samplers.
	m_shader->UnBind();
}

// Regenerates a single object. This should be called if a texture must be refreshed (e.g. image file has changed.)
void Renderer::RegenerateObject(unsigned int index)
{
	m_shader->Bind();

	if (objectsToRender[index]->HasComponent("Sprite Renderer"))
	{
		std::shared_ptr<SpriteRenderer> spriteRenderer = objectsToRender[index]->GetComponent<SpriteRenderer>();
		spriteRenderer->cTexture = GetCachedBindlessTexture(spriteRenderer);
		samplers[index] = spriteRenderer->cTexture.textureHandle;

		texturesLoaded++;
	}

	m_shader->SetUniformHandlei64vARB("u_Textures", sizeof(samplers), samplers); // Sets the shader texture slots to samplers.
	m_shader->UnBind();
}

// Gets the number of cached textures. This is for debugging in the editor only.
int Renderer::GetCachedTextureCount() const
{
	return cachedTextures.size();
}

// Creates a new quad with the transform matrix and sprite renderer colour values.
Vertex* Renderer::CreateQuad(Vertex* target, glm::mat4 transform, float texID, Vector4 color)
{
	target->Position = transform * glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f);
	target->Color = color;
	target->TextureCoords = { 0.0f, 0.0f };
	target->TextureID = texID;
	target++;

	target->Position = transform * glm::vec4(0.5f, -0.5f, 0.0f, 1.0f);
	target->Color = color;
	target->TextureCoords = { 1.0f, 0.0f };
	target->TextureID = texID;
	target++;
	
	target->Position = transform * glm::vec4(0.5f, 0.5f, 0.0f, 1.0f);
	target->Color = color;
	target->TextureCoords = { 1.0f, 1.0f };
	target->TextureID = texID;
	target++;
	
	target->Position = transform * glm::vec4(-0.5f, 0.5f, 0.0f, 1.0f);
	target->Color = color;
	target->TextureCoords = { 0.0f, 1.0f };
	target->TextureID = texID;
	target++;

	return target;
}

// Searches for a cached texture with the same file path.
LiteTexture Renderer::GetCachedBindlessTexture(std::shared_ptr<SpriteRenderer> spriteRendererComp)
{
	// Loops through each cached texture.
	for (int i = 0; i < cachedTextures.size(); i++)
	{
		// Checks if the file path matches.
		if (spriteRendererComp->cTexture.m_imagePath == cachedTextures[i].m_imagePath)
		{
			std::cout << "Used caching for " << spriteRendererComp->cTexture.m_imagePath << std::endl;
			return cachedTextures[i]; // Returns cached texture.
		}
	}

	newTextures++; // Increases the number of newly generated textures. This is for debugging only.

	LiteTexture newTexture = m_text.GenBindlessTexture(spriteRendererComp->cTexture.m_imagePath); // Generates a new texture.
	cachedTextures.push_back(newTexture); // Caches the new texture.

	return newTexture; // Returns the new texture if none was found.
}
