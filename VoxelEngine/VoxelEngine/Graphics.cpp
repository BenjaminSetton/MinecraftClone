#include "pch.h"

#include "Graphics.h"

using namespace DirectX;

Graphics::Graphics()
{
	m_D3D = nullptr;
}

Graphics::Graphics(const Graphics&){}

Graphics::~Graphics()
{
}

bool Graphics::Initialize(const int& screenWidth, const int& screenHeight, HWND windowHandle)
{
	bool initResult;

	m_D3D = new D3D;
	initResult = m_D3D->Initialize(screenWidth, screenHeight, windowHandle, VSYNC_ENABLED, FULL_SCREEN, SCREEN_FAR, SCREEN_NEAR);
	if (!initResult) return false;


	return true;
}

void Graphics::Shutdown()
{
	if (m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = nullptr;
	}
}

bool Graphics::Frame(const float& deltaTime)
{
	m_D3D->BeginScene(XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));

	// Render models, calculate shadows, render UI, etc

	// End the scene and present the swap chain
	m_D3D->EndScene();

	return true;
}