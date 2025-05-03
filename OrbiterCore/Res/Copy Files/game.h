#pragma once
#include "orbiter.h"

class Game : public RenderingLayer 
{
	public:
		void Init(GLFWwindow* window) override;

		bool OnUpdate(float deltaTime, float time) override;
		void Close() override;
	private:
		Scene currentScene;
};