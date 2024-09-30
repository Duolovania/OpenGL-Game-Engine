#pragma once
#include "Rendering/vertexarray.h"
#include "Rendering/indexbuffer.h"

#include "Rendering/texture.h"
#include "Rendering/shader.h"

#include "Math/vector.h"
#include "GameObjects/character.h"
#include "Math/vertex.h"
#include <array>

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

        void Draw() const;
        void Draw(glm::mat4 projection, glm::mat4 view, glm::vec4 colourTint);

        void RegenerateObjects();
        void RegenerateObject(unsigned int index);

        //std::vector<Character> objectsToRender;
        std::vector<std::shared_ptr<GameObject>> objectsToRender;

        int texturesLoaded = 0, newTextures = 0;
    private:
        std::unique_ptr<VertexArray> m_va;
        std::unique_ptr<IndexBuffer> m_ib;
        std::unique_ptr<VertexBuffer> m_vb;

        Texture m_text;
        std::unique_ptr<Shader> m_shader;

        Vertex* buffer;
        std::array<Vertex, 200> vertices;
        std::vector<LiteTexture> cachedTextures;

        Vertex* CreateQuad(Vertex* target, glm::mat4 transform, float texID, Vector4 color);
        LiteTexture GetCachedBindlessTexture(Character character, unsigned int index);

        GLuint64 samplers[32] = { 0, 1, 2 }; // How many texture slots.
};