#include "pch.h"

#include "Graphics.h"

Graphics::Graphics()
{
	m_D3D = nullptr;
}
Graphics::~Graphics()
{
	if (m_D3D) 
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = nullptr;
	}
}
Graphics::Graphics(const Graphics&){}

bool Graphics::Initialize(const int& screenWidth, const int& screenHeight, HWND windowHandle)
{
	return true;
}

void Graphics::Shutdown()
{
}

bool Graphics::Frame(const float& deltaTime)
{
	return true;
}