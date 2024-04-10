#include "Headers/indexbuffer.h"
#include "Headers/renderer.h"

IndexBuffer::IndexBuffer(unsigned int count)
    : count(count)
{
    
}

IndexBuffer::~IndexBuffer()
{
    GLCall(glDeleteBuffers(1, &rendererID));
}

// Generates index buffer data.
void IndexBuffer::Gen(const unsigned int* data)
{
    ASSERT(sizeof(unsigned int) == sizeof(GLuint));

    GLCall(glGenBuffers(1, &rendererID)); // How many buffers and the ID
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendererID)); // Selects buffer
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count, data, GL_STATIC_DRAW)); // Generates buffer data
    //GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW)); // Generates buffer data
}

// Selects this as current buffer.
void IndexBuffer::Bind() const
{
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendererID));
}

// Deselects buffer.
void IndexBuffer::Unbind() const
{
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}