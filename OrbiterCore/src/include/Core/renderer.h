#pragma once
#include "Rendering/vertexarray.h"
#include "Rendering/indexbuffer.h"

#include "Rendering/texture.h"
#include "Rendering/shader.h"
#include "GameObjects/gameobject.h"

#include "Math/vector.h"
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

        void Init(); // Generates essential rendering data. This must be called before draw().
        void Draw(glm::mat4 projection, glm::mat4 view, glm::vec4 colourTint); // Outputs the rendering data onto the viewport.

        void RegenerateObjects(); // Regenerates all objects. This should be called if multiple textures must be refreshed (e.g. image file has changed.)
        void RegenerateObject(unsigned int index); // Regenerates a single object. This should be called if a texture must be refreshed (e.g. image file has changed.)

        int GetCachedTextureCount() const; // Gets the number of cached textures. This is for debugging in the editor only.

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

        Vertex* CreateQuad(Vertex* target, glm::mat4 transform, float texID, Vector4 color); // Creates a new quad with the transform matrix and sprite renderer colour values.
        LiteTexture GetCachedBindlessTexture(std::shared_ptr<SpriteRenderer> spriteRendererComp); // Searches for a cached texture with the same file path.

        GLuint64 samplers[32] = { 0, 1, 2 }; // How many texture slots.
};