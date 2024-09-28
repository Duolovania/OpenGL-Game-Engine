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
	m_texture->UnBind();
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

	m_texture = std::make_unique<Texture>("Assets/Sprites/R1.png");
	m_texture->Gen();

	m_va->Unbind();
	m_texture->UnBind();
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
		if (std::shared_ptr<Character> character = dynamic_pointer_cast<Character>(objectsToRender[i]))
		{
			if (character->CheckVisibility(glm::vec2(view[3].x, view[3].y)))
			{
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(character->transform.position.x, character->transform.position.y, 0.0f))
					* glm::rotate(glm::mat4(1.0f), glm::radians(-character->transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f))
					* glm::scale(glm::mat4(1.0f), glm::vec3(character->transform.scale.x, character->transform.scale.y, 1.0f));

				m_shader->BindTexture(i, character->cTexture.textureBuffer);

				buffer = CreateQuad(buffer, transform, i, { character->color[0], character->color[1], character->color[2], character->color[3] });
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

	int samplers[32] = { 0, 1, 2 }; // How many texture slots.

	// Prepares necessary amount of slots and binds each character texture to a slot.
	for (int i = 0; i < objectsToRender.size(); i++)
	{
		samplers[i] = i;
		m_texture->Bind(i + 1);

		if (std::shared_ptr<Character> character = dynamic_pointer_cast<Character>(objectsToRender[i]))
		{
			character->cTexture.textureBuffer = GetCachedTexture(*character, i);
			m_shader->BindTexture(i, character->cTexture.textureBuffer);
		}

		texturesLoaded++;
	}

	//m_shader->SetUniform3f("ambientLight", glm::vec3(0.05, 0.05, 0.05));
	m_shader->SetUniform1iv("u_Textures", sizeof(samplers), samplers); // Sets the shader texture slots to samplers.
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

// Retrieves an existing texture if it already exists.
unsigned int Renderer::GetCachedTexture(Character character, unsigned int index)
{
	// Searches for a texture with the same file path.
	for (int i = 0; i < index; i++)
	{
		if (character.cTexture.m_imagePath == cachedTextures[i].m_imagePath)
		{
			return cachedTextures[i].textureBuffer;
		}
	}

	newTextures++;

	LiteTexture newTexture;
	newTexture.m_imagePath = character.cTexture.m_imagePath;
	newTexture.textureBuffer = m_texture->Load(character.cTexture.m_imagePath);
	cachedTextures.push_back(newTexture);

	return newTexture.textureBuffer; // Returns a new texture if none was found.
}