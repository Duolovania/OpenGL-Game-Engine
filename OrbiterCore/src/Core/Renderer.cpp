#pragma once
#include <iostream>
#include "Core/renderer.h"

void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

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

void Renderer::Init()
{
	const size_t MaxQuadCount = 200, MaxVertexCount = MaxQuadCount * 4, MaxIndexCount = MaxQuadCount * 6;

	uint32_t offset = 0;
	uint32_t indices[MaxIndexCount];

	for (size_t i = 0; i < MaxIndexCount; i += 6) // Creates as many vertices as max.
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

	m_va = std::make_unique<VertexArray>();
	m_va->Gen();

	m_ib = std::make_unique<IndexBuffer>(sizeof(indices));
	m_ib->Gen(indices);

	m_vb = std::make_unique<VertexBuffer>(MaxVertexCount * sizeof(Vertex));
	m_vb->Gen(nullptr);

	VertexBufferLayout layout;

	layout.Push<float>(3); // Position (takes 3 params).
	layout.Push<float>(4); // Colour (takes 4 params).
	layout.Push<float>(2); // TexCoords (takes 2 params).
	layout.Push<float>(1); // TexIndex (has 1 value).

	m_va->AddBuffer(*m_vb, layout);

	m_shader = std::make_unique<Shader>("../OrbiterCore/Res/Shaders/Basic.shader");
	m_shader->CreateShader();
	m_shader->Bind();

	LiteTexture newTexture = m_text.GenBindlessTexture("../OrbiterCore/Res/Default Sprites/whitesqure.png");
	cachedTextures.push_back(newTexture);

	m_va->Unbind();
	m_shader->UnBind();
	m_vb->Unbind();
	m_ib->Unbind();
}

// Outputs the data onto the viewport.
void Renderer::Draw() const
{
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	m_shader->Bind();
	m_va->Bind();
	m_ib->Bind();

	GLCall(glDrawElements(GL_TRIANGLES, m_ib->GetCount(), GL_UNSIGNED_INT, nullptr));

	m_shader->UnBind();
	m_va->Unbind();
	m_ib->Unbind();
}

// Outputs the data onto the viewport.
void Renderer::Draw(glm::mat4 projection, glm::mat4 view, glm::vec4 colourTint)
{
	buffer = vertices.data();
	m_shader->Bind();

	for (int i = 0; i < objectsToRender.size(); i++)
	{
		if (objectsToRender[i]->HasComponent("Sprite Renderer"))
		{
			std::shared_ptr<SpriteRenderer> spriteRenderer = objectsToRender[i]->GetComponent<SpriteRenderer>();

			if (spriteRenderer->CheckVisibility(glm::vec2(view[3].x, view[3].y)))
			{
				glm::mat4 transform =
					glm::translate(glm::mat4(1.0f), glm::vec3(objectsToRender[i]->transform.position.x, objectsToRender[i]->transform.position.y, 0.0f))
					* glm::rotate(glm::mat4(1.0f), glm::radians(-objectsToRender[i]->transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f))
					* glm::scale(glm::mat4(1.0f), glm::vec3(objectsToRender[i]->transform.scale.x, objectsToRender[i]->transform.scale.y, 1.0f));

				buffer = CreateQuad(buffer, transform, i, { spriteRenderer->color[0], spriteRenderer->color[1], spriteRenderer->color[2], spriteRenderer->color[3] });
			}
		}
	}

	m_vb->Bind();
	m_vb->ModifyData(vertices.size() * sizeof(Vertex), vertices.data());

	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)); // Model translation.
	glm::mat4 mvp = projection * view * model;

	m_shader->SetUniformMat4f("u_MVP", mvp);
	m_shader->SetUniform4f("u_Color", colourTint);

	//m_shader->SetUniform2f("lightPosition", glm::vec2(view[3].x, view[3].y));
	//m_shader->SetUniform2f("position", glm::vec2(view[3].x, view[3].y));

	m_va->Bind();
	m_ib->Bind();

	GLCall(glDrawElements(GL_TRIANGLES, m_ib->GetCount(), GL_UNSIGNED_INT, nullptr));

	m_shader->UnBind();
	m_vb->Unbind();
	m_ib->Unbind();
	m_va->Unbind();
}

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
			spriteRenderer->cTexture = GetCachedBindlessTexture(spriteRenderer, i);
			samplers[i] = spriteRenderer->cTexture.textureHandle;

			texturesLoaded++;
		}
	}

	//m_shader->SetUniform3f("ambientLight", glm::vec3(0.05, 0.05, 0.05));
	m_shader->SetUniformHandlei64vARB("u_Textures", sizeof(samplers), samplers); // Sets the shader texture slots to samplers.
	m_shader->UnBind();
}

void Renderer::RegenerateObject(unsigned int index)
{
	m_shader->Bind();

	if (objectsToRender[index]->HasComponent("Sprite Renderer"))
	{
		std::shared_ptr<SpriteRenderer> spriteRenderer = objectsToRender[index]->GetComponent<SpriteRenderer>();
		spriteRenderer->cTexture = GetCachedBindlessTexture(spriteRenderer, index);
		samplers[index] = spriteRenderer->cTexture.textureHandle;

		texturesLoaded++;
	}

	m_shader->SetUniformHandlei64vARB("u_Textures", sizeof(samplers), samplers); // Sets the shader texture slots to samplers.
	m_shader->UnBind();
}

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

LiteTexture Renderer::GetCachedBindlessTexture(std::shared_ptr<SpriteRenderer> spriteRendererComp, unsigned int index)
{
	// Searches for a texture with the same file path.
	for (int i = 0; i < index; i++)
	{
		if (spriteRendererComp->cTexture.m_imagePath == cachedTextures[i].m_imagePath)
		{
			std::cout << "used caching for " << spriteRendererComp->cTexture.m_imagePath << std::endl;
			return cachedTextures[i];
		}
	}

	newTextures++;

	LiteTexture newTexture = m_text.GenBindlessTexture(spriteRendererComp->cTexture.m_imagePath);
	cachedTextures.push_back(newTexture);

	return newTexture; // Returns a new texture if none was found.
}
