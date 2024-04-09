#include "Headers/VertexBuffer.h"
#include "Headers/Renderer.h"

VertexBuffer::VertexBuffer(unsigned int size) 
    : size(size)
{

}

VertexBuffer::~VertexBuffer() 
{
    GLCall(glDeleteBuffers(1, &rendererID));
}

void VertexBuffer::Gen(const void* data)
{
    GLCall(glCreateBuffers(1, &rendererID)); // How many buffers and the ID
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, rendererID)); // Selects buffer
    GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW)); // Generates buffer data
}

void VertexBuffer::Bind() const
{
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, rendererID)); // Selects buffer
}

void VertexBuffer::Unbind() const
{
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0)); // Deselects buffer
}

void VertexBuffer::ModData(const void* data)
{
    //GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW)); // Generates buffer data

    GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(data) * 1000, data));
}