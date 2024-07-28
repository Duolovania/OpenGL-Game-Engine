#pragma once
#include <glew.h>
#include "Rendering/vertexarray.h"
#include "Rendering/indexbuffer.h"
#include "Rendering/vertexbuffer.h"
#include "Rendering/vertexbufferlayout.h"
#include "Core/character.h"

#include "Rendering/texture.h"
#include <array>

#include "Rendering/shader.h"
#include "Math/vector.h"

#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))

void GLClearError();

// Use ASSERT(GLLogCall()) to debug.
bool GLLogCall(const char* function, const char* file, int line);

class Renderer
{
    public:
        Renderer();
        ~Renderer();

        void Init();

        void Clear() const;

        void Draw() const;
        void Draw(glm::mat4 projection, glm::vec2 cameraPosition, glm::vec4 colorFilter);
        std::vector<Character> objectsToRender;


        int texturesLoaded = 0, newTextures = 0;
    private:
        std::unique_ptr<VertexArray> m_va;
        std::unique_ptr<IndexBuffer> m_ib;
        std::unique_ptr<VertexBuffer> m_vb;

        std::unique_ptr<Texture> m_texture;
        std::unique_ptr<Shader> m_shader;

        Vertex* buffer;
        std::array<Vertex, 200> vertices;

        void CreateQuad(float x, float y, Vector3 size, float texID, Vector4 color);

        unsigned int GetCachedTexture(Character character, unsigned int index);
};