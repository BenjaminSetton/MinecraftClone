#include "pch.h"

#include "Graphics.h"

using namespace DirectX;

Graphics::Graphics()
{
	m_D3D = nullptr;
	m_debugCam = nullptr;
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

	m_debugCam = new Camera;
	m_debugCam->SetPosition({-0.5f, 2.5f, -5.0f});
	// We can temporarily call Render() here since camera's position and rotation isn't changing for now
	m_debugCam->ConstructMatrix();

	// Create the shader class object
	m_shader = new DefaultBlockShader;
	m_shader->CreateObjects(m_D3D->GetDevice(), L"./Shaders/DefaultBlock_VS.hlsl", L"./Shaders/DefaultBlock_PS.hlsl");

	return true;
}

void Graphics::Shutdown()
{
	if(m_shader)
	{
		m_shader->Shutdown();
		delete m_shader;
		m_shader = nullptr;
	}
	if (m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = nullptr;
	}

	if (m_debugCam) 
	{
		delete m_debugCam;
		m_debugCam = nullptr;
	}
}

bool Graphics::Frame(const float& deltaTime)
{
	m_D3D->BeginScene(XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));

	XMFLOAT3 cameraRot = m_debugCam->GetRotation();
	m_debugCam->SetRotation({ 25.0f, 10.0f, 0.0f });
	m_debugCam->ConstructMatrix();

	// Render models, calculate shadows, render UI, etc
	m_shader->Render(m_D3D->GetDeviceContext(), 36, m_D3D->GetWorldMatrix(), m_debugCam->GetViewMatrix(), m_D3D->GetProjectionMatrix());

	// End the scene and present the swap chain
	m_D3D->EndScene();

	return true;
}