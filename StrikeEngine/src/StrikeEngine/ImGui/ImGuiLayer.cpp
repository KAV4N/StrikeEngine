#include "strikepch.h"
#include "ImGuiLayer.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <backends/imgui_impl_opengl3_loader.h>

#include <StrikeEngine/Core/Application.h>


#include <GLFW/glfw3.h>

//#include <glad/glad.h>

namespace StrikeEngine {


	ImGuiLayer::ImGuiLayer()
		: Layer("ImGuiLayer"), m_Time(0.0f)
	{
	}

	ImGuiLayer::~ImGuiLayer()
	{
	}

	void ImGuiLayer::OnAttach()
	{
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

		SetupImGuiStyle();
		Application& app = Application::Get();
		GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());

		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 410");

	}

	void ImGuiLayer::Begin() 
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::End() 
	{
		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();
		io.DisplaySize = ImVec2(app.GetWindow().GetWidth(), app.GetWindow().GetHeight());

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}

	


	void ImGuiLayer::OnDetach()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::OnImGuiRender()
	{
		static bool show = true;
		ImGui::ShowDemoWindow(&show);
	}

	void ImGuiLayer::SetupImGuiStyle()
	{
		ImGuiStyle& style = ImGui::GetStyle();
		ImVec4* colors = style.Colors;

		colors[ImGuiCol_Text] = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.36f, 0.42f, 0.47f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.14f, 0.17f, 1.00f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.10f, 0.14f, 0.17f, 1.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.10f, 0.14f, 0.17f, 1.00f);
		colors[ImGuiCol_Border] = ImVec4(0.20f, 0.25f, 0.29f, 0.60f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.12f, 0.20f, 0.28f, 1.00f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.09f, 0.12f, 0.14f, 1.00f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.09f, 0.12f, 0.14f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.10f, 0.14f, 0.17f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.09f, 0.12f, 0.14f, 1.00f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.14f, 0.17f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.14f, 0.17f, 1.00f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.18f, 0.22f, 0.25f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.18f, 0.22f, 0.25f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.00f, 0.43f, 0.77f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.00f, 0.43f, 0.77f, 1.00f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.00f, 0.43f, 0.77f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(0.00f, 0.43f, 0.77f, 1.00f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.00f, 0.43f, 0.77f, 0.75f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.00f, 0.43f, 0.77f, 0.63f);
		colors[ImGuiCol_Header] = ImVec4(0.00f, 0.43f, 0.77f, 0.76f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.43f, 0.77f, 0.86f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.00f, 0.43f, 0.77f, 1.00f);
		colors[ImGuiCol_Separator] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
		colors[ImGuiCol_Tab] = ImVec4(0.08f, 0.16f, 0.28f, 1.00f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.33f, 0.70f, 1.00f, 0.80f);
		colors[ImGuiCol_TabActive] = ImVec4(0.23f, 0.49f, 0.77f, 1.00f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.26f, 0.42f, 1.00f);
		colors[ImGuiCol_DockingPreview] = ImVec4(0.33f, 0.70f, 1.00f, 0.78f);
		colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.20f, 0.21f, 1.00f);
		colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
		colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
		colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.07f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 0.43f, 0.77f, 0.35f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(0.00f, 0.43f, 0.77f, 0.95f);
		colors[ImGuiCol_NavHighlight] = ImVec4(0.00f, 0.43f, 0.77f, 0.80f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.00f, 0.43f, 0.77f, 0.80f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.25f, 0.29f, 0.73f);

		// Set rounded corners for widgets
		style.FrameRounding = 5.0f;
		style.GrabRounding = 5.0f;
	}
}
