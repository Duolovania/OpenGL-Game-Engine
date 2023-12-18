#pragma once
#include <iostream>
#include <unordered_map>
#include "glm/glm.hpp"

// Struct contains Vertex and Fragment data.
struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
};

class Shader
{
	private:
		std::string filePath;
		unsigned int rendererID;
		int GetUniformLocation(const std::string& name);

		unsigned int CompileShader(unsigned int type, const std::string& source);
		ShaderProgramSource source;

		// Searches .shader files for vertex and fragment data.
		ShaderProgramSource ParseShader(const std::string& filePath);

		//unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);

		std::unordered_map<std::string, int> uniformLocationCache;

	public:
		Shader(const std::string& filePath);
		~Shader();

		void Bind() const;
		void UnBind() const;

		void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
		void SetUniform1f(const std::string& name, float value);
		void SetUniform1i(const std::string& name, int value);

		void SetUniformMat4f(const std::string& name, const glm::mat4& matrix);

		// Assign vertex and fragment to shader.
		void CreateShader();

};