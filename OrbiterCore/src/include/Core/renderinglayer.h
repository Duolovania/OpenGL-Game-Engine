#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "Rendering/framebuffer.h"

class RenderingLayer
{
	public:
		virtual void Init(GLFWwindow* window) {};
		virtual bool OnUpdate(float deltaTime, float time) { return true; };
		virtual void Close() {};
		virtual void CleanUp() {};
		std::shared_ptr<FrameBuffer> framebuffer;
		std::shared_ptr<Shader> fbShader;
		int vsyncEnabled = 1;
};