#ifndef _D3D_H
#define _D3D_H

// Library (linker)
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

// D3D11 Includes
#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <DirectXMath.h>

class D3D
{
public:

	D3D() = default;
	D3D(const D3D&) = default;
	~D3D() = default;

	static bool Initialize(int32_t* out_screenWidth, int32_t* out_screenHeight, HWND hwnd, const bool& vsync, 
					const bool& fullscreen, const float& screenFar, const float& screenNear);
	static void Shutdown();

	static void BeginScene(DirectX::XMFLOAT4 clearToColor);
	static void EndScene();

	static ID3D11Device* GetDevice();
	static ID3D11DeviceContext* GetDeviceContext();
	static IDXGISwapChain* GetSwapChain();

	static DirectX::XMMATRIX GetWorldMatrix();
	static DirectX::XMMATRIX GetProjectionMatrix();
	static DirectX::XMMATRIX GetOrthoMatrix();

	static ID3D11RenderTargetView* GetRenderTargetView();

	static void GetVideoCardInfo(char*, int&);

	static ID3D11RenderTargetView* GetBackBuffer();
	static ID3D11DepthStencilView* GetDepthStencilView();
	static ID3D11DepthStencilState* GetDepthStencilState();

	static void TurnZBufferOn();
	static void TurnZBufferOff();

	static void ClearDepthBuffer(float value);

	static bool WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	static void SetWireframeRasterState(const bool isWireframe);

private:

	static void CreateRenderTargetView();

	static void CreateDepthStencilView(UINT width, UINT height);

	static void OnResize(LPARAM lparam);

private:

	static bool m_vsync_enabled;
	static int m_videoCardMemory;
	static char m_videoCardDescription[128];

	static float m_screenNear;
	static float m_screenFar;

	static IDXGISwapChain* m_swapChain;
	static ID3D11Device* m_device;
	static ID3D11DeviceContext* m_deviceContext;

	static ID3D11RenderTargetView* m_renderTargetView;
	static ID3D11Texture2D* m_depthStencilBuffer;

	static ID3D11DepthStencilState* m_depthStencilState;
	static ID3D11DepthStencilState* m_depthDisabledStencilState;
	static ID3D11DepthStencilView* m_depthStencilView;

	static ID3D11RasterizerState* m_defaultRasterState;
	static ID3D11RasterizerState* m_wireframeRasterState;

	static DirectX::XMMATRIX m_projectionMatrix;
	static DirectX::XMMATRIX m_worldMatrix;
	static DirectX::XMMATRIX m_orthoMatrix;

	static bool m_depthDisabled;
};

#endif

