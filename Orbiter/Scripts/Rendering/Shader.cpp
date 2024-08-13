#include "Rendering/shader.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "Core/renderer.h"

Shader::Shader(const std::string& filePath)
	:filePath(filePath), rendererID(0)
{
    source = ParseShader(filePath);
}

Shader::~Shader()
{
    GLCall(glDeleteProgram(rendererID));
}

unsigned int Shader::GetRendererID()
{
    return rendererID;
}

ShaderProgramSource Shader::ParseShader(const std::string& filePath)
{
    std::ifstream stream(filePath);

    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;

    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
                type = ShaderType::VERTEX;
            else if (line.find("fragment") != std::string::npos)
                type = ShaderType::FRAGMENT;
        }
        else
        {
            ss[(int)type] << line << '\n';
        }
    }

    // Returns vertex and fragment data
    return { ss[0].str(), ss[1].str() };
}

void Shader::CreateShader()
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, source.VertexSource);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, source.FragmentSource);

    GLCall(glAttachShader(program, vs));
    GLCall(glAttachShader(program, fs));

    GLCall(glLinkProgram(program));
    GLCall(glValidateProgram(program));

    GLCall(glDeleteShader(vs));
    GLCall(glDeleteShader(fs));

    rendererID = program;
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    GLCall(glShaderSource(id, 1, &src, nullptr));
    GLCall(glCompileShader(id));

    int result;
    GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));

    // Error handler
    if (result == GL_FALSE)
    {
        int length;
        GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
        char* message = (char*)_malloca(length * sizeof(char));

        GLCall(glGetShaderInfoLog(id, length, &length, message));
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
        std::cout << message << std::endl;

        GLCall(glDeleteShader(id));
        return 0;
    }

    return id;
}

void Shader::Bind() const 
{
    GLCall(glUseProgram(rendererID));
}

void Shader::UnBind() const 
{
    GLCall(glUseProgram(0));
}

void Shader::SetUniform4f(const std::string& name, const glm::vec4& value) 
{
    GLCall(glUniform4f(GetUniformLocation(name), value.x, value.y, value.z, value.w));
}

void Shader::SetUniform3f(const std::string& name, const glm::vec3& value)
{
    GLCall(glUniform3f(GetUniformLocation(name), value.x, value.y, value.z));
}

void Shader::SetUniform2f(const std::string& name, const glm::vec2& value)
{
    GLCall(glUniform2f(GetUniformLocation(name), value.x, value.y));
}

void Shader::SetUniform1f(const std::string& name, float value)
{
    GLCall(glUniform1f(GetUniformLocation(name), value));
}

void Shader::SetUniform1i(const std::string& name, int value)
{
    GLCall(glUniform1i(GetUniformLocation(name), value));
}

void Shader::SetUniform1iv(const std::string& name, int count, const int* value)
{
    GLCall(glUniform1iv(GetUniformLocation(name), count, value));
}

void Shader::SetUniformMat4f(const std::string& name, const glm::mat4& matrix)
{
    GLCall(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]));
}

GLint Shader::GetUniformLocation(const std::string& name) const
{
    if (uniformLocationCache.find(name) != uniformLocationCache.end())
        return uniformLocationCache[name];

    GLCall(GLint location = glGetUniformLocation(rendererID, name.c_str()));
    if (location == -1)
        std::cout << "Warning: \n    Uniform: '" << name << "' doesn't exist." << std::endl;
    
    uniformLocationCache[name] = location;
    return location;
}

void Shader::BindTexture(unsigned int index, unsigned int value)
{
    GLCall(glBindTextureUnit(index, value));
}