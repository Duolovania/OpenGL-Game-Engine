#include "Headers/shader.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "Headers/Renderer.h"

Shader::Shader(const std::string& filePath)
	:filePath(filePath), rendererID(0)
{
    //ShaderProgramSource source = ParseShader(filePath);
    source = ParseShader(filePath);
}

Shader::~Shader()
{
    GLCall(glDeleteProgram(rendererID));
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

//unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
//{
//    unsigned int program = glCreateProgram();
//    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
//    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
//
//    GLCall(glAttachShader(program, vs));
//    GLCall(glAttachShader(program, fs));
//
//    GLCall(glLinkProgram(program));
//    GLCall(glValidateProgram(program));
//
//    GLCall(glDeleteShader(vs));
//    GLCall(glDeleteShader(fs));
//
//    return program;
//}

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

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3) 
{
    GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
}

void Shader::SetUniform1f(const std::string& name, float value)
{
    GLCall(glUniform1f(GetUniformLocation(name), value));
}

void Shader::SetUniform1i(const std::string& name, int value)
{
    GLCall(glUniform1i(GetUniformLocation(name), value));
}

void Shader::SetUniformMat4f(const std::string& name, const glm::mat4& matrix)
{
    GLCall(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]));
}

int Shader::GetUniformLocation(const std::string& name)
{
    if (uniformLocationCache.find(name) != uniformLocationCache.end())
        return uniformLocationCache[name];

    GLCall(int location = glGetUniformLocation(rendererID, name.c_str()));
    if (location == -1)
        std::cout << "Warning: unfiorm: '" << name << "' doesn't exist." << std::endl;
    
    uniformLocationCache[name] = location;
    return location;
}