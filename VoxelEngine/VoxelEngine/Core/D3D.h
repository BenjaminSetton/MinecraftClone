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

	bool Initialize(const int& screenWidth, const int& screenHeight, HWND hwnd, const bool& vsync, 
					const bool& fullscreen, const float& screenFar, const float& screenNear);
	void Shutdown();

	void BeginScene(DirectX::XMFLOAT4 clearToColor);
	void EndScene();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();
	IDXGISwapChain* GetSwapChain();

	const DirectX::XMMATRIX GetWorldMatrix() const;

	const DirectX::XMMATRIX GetProjectionMatrix() const;
	const DirectX::XMMATRIX GetOrthoMatrix() const;
	ID3D11RenderTargetView* GetRenderTargetView() const;

	void GetVideoCardInfo(char*, int&);

	void TurnZBufferOn();
	void TurnZBufferOff();

	void ClearDepthBuffer(float value);

	bool WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:

	void CreateRenderTargetView();

	void CreateDepthStencilView(UINT width, UINT height);

	void OnResize(LPARAM lparam);

private:

	bool m_vsync_enabled;
	int m_videoCardMemory;
	char m_videoCardDescription[128];

	float m_screenNear;
	float m_screenFar;

	IDXGISwapChain* m_swapChain = nullptr;
	ID3D11Device* m_device = nullptr;
	ID3D11DeviceContext* m_deviceContext = nullptr;

	ID3D11RenderTargetView* m_renderTargetView = nullptr;
	ID3D11Texture2D* m_depthStencilBuffer = nullptr;

	ID3D11DepthStencilState* m_depthStencilState = nullptr;
	ID3D11DepthStencilState* m_depthDisabledStencilState = nullptr;
	ID3D11DepthStencilView* m_depthStencilView = nullptr;
	ID3D11RasterizerState* m_defaultRasterState = nullptr;
	ID3D11RasterizerState* m_wireframeRasterState = nullptr;

	DirectX::XMMATRIX m_projectionMatrix;
	DirectX::XMMATRIX m_worldMatrix;
	DirectX::XMMATRIX m_orthoMatrix;


};

#endif

