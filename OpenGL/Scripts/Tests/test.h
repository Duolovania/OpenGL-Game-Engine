#pragma once
//#include <glew.h>
#include <glad/glad.h>

#include <glfw3.h>
#include <vector>
#include <iostream>
#include <functional>
#include "imgui/imgui.h"
#include "gtc/matrix_transform.hpp"

namespace testSpace
{
	class Test
	{
		public:
			Test() {};
			virtual ~Test() {};

			virtual void OnUpdate(float deltaTime) {};
			virtual void OnRender(glm::mat4 proj) {};
			virtual void OnImGuiRender() {};
	};

	class TestMenu : public Test
	{
		public:
			TestMenu(Test*& currentTest);

			void OnImGuiRender() override;

			template<typename T>
			void RegisterTest(const std::string& name)
			{
				std::cout << "Registering Test" << name << std::endl;
				tests.push_back(std::make_pair(name, []() { return new T(); }));
			};

		private:
			Test*& currentTest;
			std::vector<std::pair<std::string, std::function<Test*()>>> tests;
	};
}