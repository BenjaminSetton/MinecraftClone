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

	D3D();
	D3D(const D3D&);
	~D3D();

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

	void OnResize(LPARAM lparam);

private:

	bool m_vsync_enabled;
	int m_videoCardMemory;
	char m_videoCardDescription[128];

	float m_screenNear;
	float m_screenFar;

	IDXGISwapChain* m_swapChain;
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_deviceContext;

	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11Texture2D* m_depthStencilBuffer;

	ID3D11DepthStencilState* m_depthStencilState;
	ID3D11DepthStencilState* m_depthDisabledStencilState;
	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11RasterizerState* m_defaultRasterState;
	ID3D11RasterizerState* m_wireframeRasterState;

	DirectX::XMMATRIX m_projectionMatrix;
	DirectX::XMMATRIX m_worldMatrix;
	DirectX::XMMATRIX m_orthoMatrix;


};

#endif

