#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "Rendering/framebuffer.h"

// This class is used to automatically trigger editor, game, and launcher methods from the core.
class RenderingLayer
{
	public:
		/**
		* @brief The setup method. This will be called at the start of the program.
		* 
		* @param window the window data.
		*/
		virtual void Init(GLFWwindow* window) {};

		/**
		* @brief The constant loop method. This will be called after 'Init' and be executed frame-by-frame.
		*
		* @param deltaTime the time since the last frame.
		* @param time the overall application time.
		*/
		virtual bool OnUpdate(float deltaTime, float time) { return true; };

		/**
		* @brief The close method. This will be called at the end of the program.
		*/
		virtual void Close() {};

		std::shared_ptr<FrameBuffer> framebuffer;
		std::shared_ptr<Shader> fbShader;
		int vsyncEnabled = 1;
};