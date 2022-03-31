#ifndef _IMGUILAYER_H
#define _IMGUILAYER_H

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_win32.h"
#include "../imgui/imgui_impl_dx11.h"
#include "DirectXMath.h"

#include "ImGuiDrawData.h"

class ImGuiLayer
{
public:

	ImGuiLayer() = default;
	ImGuiLayer(const ImGuiLayer& layer) = default;
	~ImGuiLayer() = default;

	void Initialize(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* context);

	void BeginFrame();

	void Draw();

	void EndFrame();

	void Shutdown();

	bool WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:

};

#endif