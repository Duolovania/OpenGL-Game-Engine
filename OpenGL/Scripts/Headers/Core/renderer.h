#pragma once
#include <glew.h>
#include "Rendering/vertexarray.h"
#include "Rendering/indexbuffer.h"
#include "Rendering/vertexbuffer.h"
#include "Rendering/vertexbufferlayout.h"

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
        void ClearVertices();

        void Draw() const;
        void Draw(glm::mat4 projection, glm::vec4 colorFilter);

        void Gen(glm::mat4 projection, glm::vec4 colorFilter = glm::vec4(1.0, 1.0, 1.0, 1.0));

        void ChangeTexture(int value);
        void CreateQuad(float x, float y, float texID, Vector4 color);

    private:
        std::unique_ptr<VertexArray> m_va;
        std::unique_ptr<IndexBuffer> m_ib;
        std::unique_ptr<VertexBuffer> m_vb;

        std::unique_ptr<Shader> m_shader;

        Vertex* buffer;
        std::array<Vertex, 200> vertices;

        unsigned int namjasTexture, shrekTexture, monkeyTexture;
};