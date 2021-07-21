#ifndef _IMGUILAYER_H
#define _IMGUILAYER_H

#include "../../imgui/imgui.h"
#include "../../imgui/imgui_impl_win32.h"
#include "../../imgui/imgui_impl_dx11.h"
#include "DirectXMath.h"

class ImGuiLayer
{
public:

	ImGuiLayer();
	ImGuiLayer(const ImGuiLayer& layer);
	~ImGuiLayer();

	void Initialize(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* context);

	void Frame();

	void Shutdown();

	bool WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:

};

#endif