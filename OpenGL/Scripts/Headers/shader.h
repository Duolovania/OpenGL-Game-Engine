#pragma once
#include <iostream>

class Shader
{
	private:

	public:
		Shader(const std::string& filename);
		~Shader();

		void Bind() const;
		void UnBind() const;

		//void 
};