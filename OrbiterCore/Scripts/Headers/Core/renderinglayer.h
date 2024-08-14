#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "Rendering/framebuffer.h"

class RenderingLayer
{
	public:
		virtual void Init(GLFWwindow* window) {};
		virtual void OnUpdate(float deltaTime) {};
		virtual void Close() {};
		virtual void CleanUp() {};
		std::shared_ptr<FrameBuffer> framebuffer;
};