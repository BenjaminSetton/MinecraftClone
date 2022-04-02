#ifndef _IMGUILAYER_H
#define _IMGUILAYER_H

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_win32.h"
#include "../imgui/imgui_impl_dx11.h"
#include "DirectXMath.h"

#include "ImGuiDrawData.h"

// Declares a base class, from which derived classes can implement ImGui functionality
class ImGuiLayer
{
public:

	ImGuiLayer() = delete;
	ImGuiLayer(const ImGuiLayer& layer) = delete;
	~ImGuiLayer() = delete;

	static void Initialize(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* context);

	static void BeginFrame();

	static void Draw();

	static void EndFrame();

	static void Shutdown();

	static bool WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:

};

#endif