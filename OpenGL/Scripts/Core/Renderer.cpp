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

	/*m_shader = std::make_unique<Shader>("Res/Shaders/Basic.shader");
	m_shader->CreateShader();
	m_shader->Bind();*/

	Character character1 = Character("namjas.JPG");
	Character character2 = Character("B happy.png");
	Character character3 = Character("shronk.jpg");
	Character character4 = Character("ForestE.png");
	Character character5 = Character("yoza2.jpg");
	Character character6 = Character("hronyman.gif");
	Character character7 = Character("johndinner.png");

	objectsToRender.push_back(character1);
	objectsToRender.push_back(character2); 
	objectsToRender.push_back(character3);
	objectsToRender.push_back(character4);
	objectsToRender.push_back(character5);
	objectsToRender.push_back(character6);
	objectsToRender.push_back(character7);

	m_texture = std::make_unique<Texture>("Res/Textures/namjas.JPG");
	m_texture->Gen();

	int samplers[100] = { 0, 1, 2 }; // How many texture slots.
	
	// Prepares necessary amount of slots and binds each character texture to a slot.
	for (int i = 0; i < objectsToRender.size(); i++)
	{
		objectsToRender[i].m_shader = std::make_unique<Shader>("Res/Shaders/Basic.shader");

		objectsToRender[i].m_shader->CreateShader();
		objectsToRender[i].m_shader->Bind();

		samplers[i] = i;
		m_texture->Bind(i + 1);

		objectsToRender[i].texture = GetCachedTexture(objectsToRender[i], i);
		//m_shader->BindTexture(i, objectsToRender[i].texture);
		objectsToRender[i].m_shader->BindTexture(i, objectsToRender[i].texture);
		objectsToRender[i].m_shader->SetUniform1iv("u_Textures", sizeof(samplers), samplers); // Sets the shader texture slots to samplers.

		texturesLoaded++;
	}

	//m_shader->SetUniform1iv("u_Textures", sizeof(samplers), samplers); // Sets the shader texture slots to samplers.

	objectsToRender[0].transform.position.x = 0;
	objectsToRender[1].transform.position.x = 100;
	objectsToRender[2].transform.position.x = 200;
	objectsToRender[3].transform.position.x = 300;
	objectsToRender[4].transform.position.x = 400;
	objectsToRender[5].transform.position.x = 500;
	objectsToRender[6].transform.position.x = -100;

	for (int i = 0; i < objectsToRender.size(); i++)
	{
		objectsToRender[i].transform.scale = Vector3(100, 100, 0);
	}

	m_va->Unbind();
	m_texture->UnBind();
	//m_shader->UnBind();
	m_vb->Unbind();
	m_ib->Unbind();
}

// Outputs the data onto the viewport.
void Renderer::Draw() const
{
	m_shader->Bind();
	m_va->Bind();
	m_ib->Bind();

	GLCall(glDrawElements(GL_TRIANGLES, m_ib->GetCount(), GL_UNSIGNED_INT, nullptr));
}

// Outputs the data onto the viewport.
void Renderer::Draw(glm::mat4 projection, glm::vec2 cameraPosition) 
{
	for (int i = 0; i < objectsToRender.size(); i++)
	{
		if (objectsToRender[i].CheckVisibility(cameraPosition))
		{
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(objectsToRender[i].transform.position.x, objectsToRender[i].transform.position.y, 0.0f))
				* glm::rotate(glm::mat4(1.0f), glm::radians(-objectsToRender[i].transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f))
				* glm::scale(glm::mat4(1.0f), glm::vec3(objectsToRender[i].transform.scale.x, objectsToRender[i].transform.scale.y, 1.0f));
			
			CreateQuad(transform, i, { 1.0f, 1.0f, 1.0f, 1.0f });

			objectsToRender[i].m_shader->Bind();
			objectsToRender[i].m_shader->SetUniform4f("u_Color", objectsToRender[i].color);
			objectsToRender[i].m_shader->SetUniformMat4f("u_MVP", projection);

			m_vb->Bind();
			m_vb->ModifyData(vertices.size() * sizeof(Vertex), vertices.data());

			m_va->Bind();
			m_ib->Bind();

			GLCall(glDrawElements(GL_TRIANGLES, m_ib->GetCount(), GL_UNSIGNED_INT, nullptr));
		}
	}
}

// Resets the viewport.
void Renderer::Clear() const
{
    GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void Renderer::CreateQuad(glm::mat4 transform, float texID, Vector4 color)
{
	buffer = vertices.data(); // Clears all vertices generated.

	buffer->Position = transform * glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f);
	buffer->Color = color;
	buffer->TextureCoords = { 0.0f, 0.0f };
	buffer->TextureID = texID;
	buffer++;

	buffer->Position = transform * glm::vec4(0.5f, -0.5f, 0.0f, 1.0f);
	buffer->Color = color;
	buffer->TextureCoords = { 1.0f, 0.0f };
	buffer->TextureID = texID;
	buffer++;

	buffer->Position = transform * glm::vec4(0.5f, 0.5f, 0.0f, 1.0f);
	buffer->Color = color;
	buffer->TextureCoords = { 1.0f, 1.0f };
	buffer->TextureID = texID;
	buffer++;

	buffer->Position = transform * glm::vec4(-0.5f, 0.5f, 0.0f, 1.0f);
	buffer->Color = color;
	buffer->TextureCoords = { 0.0f, 1.0f };
	buffer->TextureID = texID;
	buffer++;
}

// Retrieves an existing texture if it already exists.
unsigned int Renderer::GetCachedTexture(Character character, unsigned int index)
{
	// Searches for a texture with the same file path.
	for (int i = 0; i < index; i++)
	{
		if (character.m_imagePath == objectsToRender[i].m_imagePath)
		{
			return objectsToRender[i].texture;
		}
	}

	newTextures++;
	return m_texture->Load(character.m_imagePath); // Returns a new texture if none was found.
}