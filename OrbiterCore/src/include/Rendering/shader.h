#pragma once
#include <iostream>
#include <unordered_map>
#include "glm/glm.hpp"
#include <glad/glad.h>

// Struct contains Vertex and Fragment data.
struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
};

class Shader
{
	public:
		Shader(const std::string& filePath);
		~Shader();

		void Bind() const;
		void UnBind() const;

		void SetUniform4f(const std::string& name, const glm::vec4& value);
		void SetUniform3f(const std::string& name, const glm::vec3& value);
		void SetUniform2f(const std::string& name, const glm::vec2& value);

		void SetUniform1f(const std::string& name, float value);
		void SetUniform1i(const std::string& name, int value);
		void SetUniform1iv(const std::string& name, int count, const int* value);

		void SetUniformHandlei64ARB(const std::string& name, const GLuint64 value); // Set bindless texture uniform.
		void SetUniformHandlei64vARB(const std::string& name, int count, const GLuint64* value); // Set bindless texture array uniform.

		void SetUniformMat4f(const std::string& name, const glm::mat4& matrix);
		void CreateShader(); // Assign vertex and fragment to shader.

		void BindTexture(unsigned int index, unsigned int value);

		unsigned int GetRendererID();
	private:
		std::string filePath;
		unsigned int rendererID;

		mutable std::unordered_map<std::string, GLint> uniformLocationCache;
		ShaderProgramSource source;

		GLint GetUniformLocation(const std::string& name) const;
		ShaderProgramSource ParseShader(const std::string& filePath); // Searches .shader files for vertex and fragment data.

		unsigned int CompileShader(unsigned int type, const std::string& source);
};