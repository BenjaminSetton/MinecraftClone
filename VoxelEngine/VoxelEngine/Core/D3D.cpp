#include "../Misc/pch.h"

#include "D3D.h"
#include "../Utility/Utility.h"

using namespace DirectX;

bool D3D::m_vsync_enabled;
int D3D::m_videoCardMemory;
char D3D::m_videoCardDescription[128];

float D3D::m_screenNear;
float D3D::m_screenFar;

IDXGISwapChain* D3D::m_swapChain = nullptr;
ID3D11Device* D3D::m_device = nullptr;
ID3D11DeviceContext* D3D::m_deviceContext = nullptr;

ID3D11RenderTargetView* D3D::m_renderTargetView = nullptr;
ID3D11Texture2D* D3D::m_depthStencilBuffer = nullptr;

ID3D11DepthStencilState* D3D::m_depthStencilState = nullptr;
ID3D11DepthStencilState* D3D::m_depthDisabledStencilState = nullptr;
ID3D11DepthStencilView* D3D::m_depthStencilView = nullptr;
ID3D11RasterizerState* D3D::m_defaultRasterState = nullptr;
ID3D11RasterizerState* D3D::m_wireframeRasterState = nullptr;

DirectX::XMMATRIX D3D::m_projectionMatrix = XMMatrixIdentity();
DirectX::XMMATRIX D3D::m_worldMatrix = XMMatrixIdentity();
DirectX::XMMATRIX D3D::m_orthoMatrix = XMMatrixIdentity();

bool D3D::m_depthDisabled = false;

bool D3D::Initialize(int32_t* out_screenWidth, int32_t* out_screenHeight, HWND hwnd, const bool& vsync,
					 const bool& fullscreen, const float& screenFar, const float& screenNear)
{
	HRESULT result;
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	unsigned int numModes = 0;
	unsigned int i, numerator, denominator, stringLength;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;
	D3D_FEATURE_LEVEL featureLevel;
	D3D11_RASTERIZER_DESC defaultRasterDesc;
	D3D11_RASTERIZER_DESC wireframeRasterDesc;


	// Store the vsync setting.
	m_vsync_enabled = vsync;

	// Store the other window / graphics settings that affect D3D devices or objects
	m_screenNear = screenNear;
	m_screenFar = screenFar;

#pragma region _VIDEO_CARD

	// Create a DirectX graphics interface factory.
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result)){ return false; }

	// Use the factory to create an adapter for the primary graphics interface (video card).
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result)){ return false; }

	// Enumerate the primary adapter output (monitor).
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result)){ return false; }

	// Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(result)){ return false; }

	// Create a list to hold all the possible display modes for this monitor/video card combination.
	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList){ return false; }

	// Now fill the display mode list structures.
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(result)){ return false; }

	// Now go through all the display modes and find the one that matches the screen width and height.
	// When a match is found store the numerator and denominator of the refresh rate for that monitor.
	//for (i = 0; i < numModes; i++)
	//{
	//	if (displayModeList[i].Width == (UINT)screenWidth)
	//	{
	//		if (displayModeList[i].Height == (UINT)screenHeight)
	//		{
	//		}
	//	}
	//}

	// Default refresh rate of 60FPS
	numerator = 60; //displayModeList[i].RefreshRate.Numerator;
	denominator = 1; //displayModeList[i].RefreshRate.Denominator;

	// Get the adapter (video card) description.
	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result))
	{
		return false;
	}

	// Store the dedicated video card memory in megabytes.
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// Convert the name of the video card to a character array and store it.
	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0) { return false; }

	// Release the display mode list.
	delete[] displayModeList;
	displayModeList = 0;

	// Release the adapter output.
	adapterOutput->Release();
	adapterOutput = nullptr;

	// Release the adapter.
	adapter->Release();
	adapter = nullptr;

	// Release the factory.
	factory->Release();
	factory = nullptr;

#pragma endregion

#pragma region _D3D_OBJECT_INIT

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	swapChainDesc.BufferCount = 2;

	// Set the width and height of the back buffer.
	swapChainDesc.BufferDesc.Width = 0;
	swapChainDesc.BufferDesc.Height = 0;

	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // Set regular 32-bit surface for the back buffer.

	// Set the refresh rate of the back buffer.
	if (m_vsync_enabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;//numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;// denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	// Set the usage of the back buffer.
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = hwnd; // Set the handle for the window to render to.

	swapChainDesc.SampleDesc.Count = 1; // Turn multisampling off.
	swapChainDesc.SampleDesc.Quality = 0;

	swapChainDesc.Windowed = !fullscreen; // Set to full screen or windowed mode.

	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // Set the scan line ordering and scaling to unspecified.
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// Discard the back buffer contents after presenting.
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	// Don't set the advanced flags.
	swapChainDesc.Flags = 0;

	// Set the feature level to DirectX 11.
	featureLevel = D3D_FEATURE_LEVEL_11_0;

	// Create the swap chain, Direct3D device, and Direct3D device context.
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL,
#ifdef _DEBUG
		D3D11_CREATE_DEVICE_DEBUG,
#else
		0, 
#endif
		&featureLevel, 1,
		D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);
	if (FAILED(result)){ return false; }

	// Retrieve the runtime-generated width and height values
	// These should be considered to be the "real" width/height values
	HRESULT hr = m_swapChain->GetDesc(&swapChainDesc);
	VX_ASSERT(!FAILED(hr));
	*out_screenWidth = swapChainDesc.BufferDesc.Width;
	*out_screenHeight = swapChainDesc.BufferDesc.Height;


	// Create the depth/stencil view and all of it's related objects
	CreateDepthStencilView(*out_screenWidth, *out_screenHeight);

	// Setup the default raster description which will determine how and what polygons will be drawn.
	defaultRasterDesc.AntialiasedLineEnable = false;
	defaultRasterDesc.CullMode = D3D11_CULL_BACK;
	defaultRasterDesc.DepthBias = 0;
	defaultRasterDesc.DepthBiasClamp = 0.0f;
	defaultRasterDesc.DepthClipEnable = true;
	defaultRasterDesc.FillMode = D3D11_FILL_SOLID;
	defaultRasterDesc.FrontCounterClockwise = false;
	defaultRasterDesc.MultisampleEnable = false;
	defaultRasterDesc.ScissorEnable = false;
	defaultRasterDesc.SlopeScaledDepthBias = 0.0f;

	// Create the rasterizer state from the description we just filled out.
	result = m_device->CreateRasterizerState(&defaultRasterDesc, &m_defaultRasterState);
	if (FAILED(result)){ return false; }

	// Setup the wireframe raster description.
	wireframeRasterDesc.AntialiasedLineEnable = true;
	wireframeRasterDesc.CullMode = D3D11_CULL_NONE;
	wireframeRasterDesc.DepthBias = 0;
	wireframeRasterDesc.DepthBiasClamp = 0.0f;
	wireframeRasterDesc.DepthClipEnable = true;
	wireframeRasterDesc.FillMode = D3D11_FILL_WIREFRAME;
	wireframeRasterDesc.FrontCounterClockwise = false;
	wireframeRasterDesc.MultisampleEnable = false;
	wireframeRasterDesc.ScissorEnable = false;
	wireframeRasterDesc.SlopeScaledDepthBias = 0.0f;

	// Create the rasterizer state from the description we just filled out.
	result = m_device->CreateRasterizerState(&wireframeRasterDesc, &m_wireframeRasterState);
	if (FAILED(result)) { return false; }

	// Now set the default rasterizer state.
	m_deviceContext->RSSetState(m_defaultRasterState);

	CreateRenderTargetView();

	// Setup the viewport for rendering.
	D3D11_VIEWPORT viewport;
	viewport.Width = static_cast<float>(*out_screenWidth);
	viewport.Height = static_cast<float>(*out_screenHeight);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	// Create the viewport.
	m_deviceContext->RSSetViewports(1, &viewport);

	// Create the projection matrix for 3D rendering.
	m_projectionMatrix = XMMatrixPerspectiveFovLH(XM_PIDIV4, 
		(float)*out_screenWidth / *out_screenHeight, screenNear, screenFar);

	// Create an orthographic projection matrix for 2D rendering.
	m_orthoMatrix = XMMatrixOrthographicOffCenterLH(
		(-static_cast<int32_t>(*out_screenWidth) / 20.0f), (static_cast<int32_t>(*out_screenWidth) / 20.0f),
		(-static_cast<int32_t>(*out_screenHeight) / 20.0f), (static_cast<int32_t>(*out_screenHeight) / 20.0f),
		screenNear, screenFar);
	
	// Initialize the world matrix to the identity matrix.
	m_worldMatrix = XMMatrixIdentity();


#pragma endregion

	return true;
}

void D3D::Shutdown()
{
	// Before shutting down set to windowed mode or when you release the swap chain it will throw an exception.
	if (m_swapChain)
	{
		m_swapChain->SetFullscreenState(false, NULL);
	}

	if (m_depthDisabledStencilState)
	{
		m_depthDisabledStencilState->Release();
		m_depthDisabledStencilState = nullptr;
	}

	if (m_defaultRasterState)
	{
		m_defaultRasterState->Release();
		m_defaultRasterState = nullptr;
	}

	if (m_wireframeRasterState)
	{
		m_wireframeRasterState->Release();
		m_wireframeRasterState = nullptr;
	}

	if (m_depthStencilView)
	{
		m_depthStencilView->Release();
		m_depthStencilView = nullptr;
	}

	if (m_depthStencilState)
	{
		m_depthStencilState->Release();
		m_depthStencilState = nullptr;
	}

	if (m_depthStencilBuffer)
	{
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = nullptr;
	}

	if (m_renderTargetView)
	{
		m_renderTargetView->Release();
		m_renderTargetView = nullptr;
	}

	if (m_deviceContext)
	{
		m_deviceContext->Release();
		m_deviceContext = nullptr;
	}

	if (m_device)
	{
		m_device->Release();
		m_device = nullptr;
	}

	if (m_swapChain)
	{
		m_swapChain->Release();
		m_swapChain = nullptr;
	}
}

void D3D::BeginScene(XMFLOAT4 clearToColor)
{
	float color[4] = { clearToColor.x, clearToColor.y, clearToColor.z, clearToColor.w };

	// Clear the back buffer.
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);

	// Clear the depth buffer.
	// NOTE: Consider maybe clearing the stencil buffer as well?
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void D3D::EndScene()
{
	// Present the back buffer to the screen since rendering is complete.
	if (m_vsync_enabled)
	{
		// Lock to screen refresh rate.
		m_swapChain->Present(1, 0);
	}
	else
	{
		// Present as fast as possible.
		m_swapChain->Present(0, 0);
	}
}

ID3D11Device* D3D::GetDevice() { return m_device; }

ID3D11DeviceContext* D3D::GetDeviceContext() { return m_deviceContext; }

IDXGISwapChain* D3D::GetSwapChain() { return m_swapChain; }

XMMATRIX D3D::GetWorldMatrix() { return m_worldMatrix; }
XMMATRIX D3D::GetProjectionMatrix() { return m_projectionMatrix; }
XMMATRIX D3D::GetOrthoMatrix() { return m_orthoMatrix; }

ID3D11RenderTargetView* D3D::GetRenderTargetView() { return m_renderTargetView; }

void D3D::GetVideoCardInfo(char* videoCardDescription, int& videoCardMemory)
{
	videoCardDescription = m_videoCardDescription;
	videoCardMemory = m_videoCardMemory;
}

ID3D11RenderTargetView* D3D::GetBackBuffer() { return m_renderTargetView; }

ID3D11DepthStencilView* D3D::GetDepthStencilView() { return m_depthStencilView; }

ID3D11DepthStencilState* D3D::GetDepthStencilState()
{
	if (m_depthDisabled) return m_depthDisabledStencilState;
	else return m_depthStencilState;
}

void D3D::TurnZBufferOn()
{
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);
	m_depthDisabled = false;
}
void D3D::TurnZBufferOff()
{
	m_deviceContext->OMSetDepthStencilState(m_depthDisabledStencilState, 1);
	m_depthDisabled = true;
}

void D3D::ClearDepthBuffer(float value)
{
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, value, 0.0f);
}

bool D3D::WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch(msg)
	{
	case WM_SIZE:
		if (m_device != NULL && wparam != SIZE_MINIMIZED)
		{
			OnResize(lparam);
		}
		return true;
	}

	return false;
}

void D3D::SetWireframeRasterState(const bool isWireframe)
{
	isWireframe ? m_deviceContext->RSSetState(m_wireframeRasterState) : m_deviceContext->RSSetState(m_defaultRasterState);
}

void D3D::CreateRenderTargetView()
{
	HRESULT hr;
	ID3D11Texture2D* backBufferPtr;

	// Clear the render targets
	m_deviceContext->OMSetRenderTargets(0, 0, 0);
	if (m_renderTargetView) { m_renderTargetView->Release(); m_renderTargetView = NULL; }

	// Get the pointer to the back buffer.
	hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	VX_ASSERT(!FAILED(hr));

	//D3D11_DSV_DIMENSION_TEXTURE2D
	hr = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
	VX_ASSERT(!FAILED(hr));

	// Set the newly-created render target
	if (m_depthStencilView)
		m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
	else
	{
		VX_LOG_WARN("Depth buffer is not bound!");
		m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, NULL);
	}

	backBufferPtr->Release();
	backBufferPtr = nullptr;
}

void D3D::CreateDepthStencilView(UINT width, UINT height)
{
	HRESULT hr;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;

	// Release the previous objects, if necessary
	if (m_depthDisabledStencilState){ m_depthDisabledStencilState->Release(); m_depthDisabledStencilState = nullptr; }
	if (m_depthStencilBuffer) { m_depthStencilBuffer->Release(); m_depthStencilBuffer = nullptr; }
	if (m_depthStencilState) { m_depthStencilState->Release(); m_depthStencilState = nullptr; }
	if (m_depthStencilView) { m_depthStencilView->Release(); m_depthStencilView = nullptr; }

	// Initialize the description of the depth buffer.
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// Set up the description of the depth buffer.
	depthBufferDesc.Width = width;
	depthBufferDesc.Height = height;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// Create the texture for the depth buffer using the filled out description.
	hr = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
	VX_ASSERT(!FAILED(hr));

	// Initialize the description of the stencil state.
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// Set up the description of the stencil state.
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create the depth stencil state.
	hr = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	VX_ASSERT(!FAILED(hr));

	// Set the depth stencil state.
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

	// Initialize the depth stencil view.
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// Set up the depth stencil view description.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Create the depth stencil view.
	hr = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	VX_ASSERT(!FAILED(hr));

	// Clear the second depth stencil state before setting the parameters.
	ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));

	// Now create a second depth stencil state which turns off the Z buffer for 2D rendering.  The only difference is 
	// that DepthEnable is set to false, all other parameters are the same as the other depth stencil state.
	depthDisabledStencilDesc.DepthEnable = false;
	depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthDisabledStencilDesc.StencilEnable = true;
	depthDisabledStencilDesc.StencilReadMask = 0xFF;
	depthDisabledStencilDesc.StencilWriteMask = 0xFF;
	depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create the state using the device.
	hr = m_device->CreateDepthStencilState(&depthDisabledStencilDesc, &m_depthDisabledStencilState);
	VX_ASSERT(!FAILED(hr));
}

void D3D::OnResize(LPARAM lparam)
{
	UINT newWidth = LOWORD(lparam);
	UINT newHeight = HIWORD(lparam);

	// Release all outstanding references to the back buffer before calling ResizeBuffers
	if (m_renderTargetView) { m_renderTargetView->Release(); m_renderTargetView = NULL; }

	HRESULT hr = m_swapChain->ResizeBuffers(0, newWidth, newHeight, DXGI_FORMAT_UNKNOWN, 0);
	VX_ASSERT(!FAILED(hr));

	CreateDepthStencilView(newWidth, newHeight);
	CreateRenderTargetView();

	// Resize the projection matrix
	m_projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(XM_PIDIV4, (float)16.0f / 9.0f, m_screenNear, m_screenFar);

	// Resize the viewport
	D3D11_VIEWPORT viewport;
	viewport.Width = static_cast<float>(newWidth);
	viewport.Height = static_cast<float>(newHeight);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	m_deviceContext->RSSetViewports(1, &viewport);
}