#include "../Misc/pch.h"

#include "Graphics.h"
#include "../Utility/Utility.h"

using namespace DirectX;

Graphics::Graphics()
{
	m_D3D = nullptr;
	m_debugCam = nullptr;
	m_shader = nullptr;
	m_textureManager = nullptr;
	m_imGuiLayer = nullptr;
}

Graphics::Graphics(const Graphics&){}

Graphics::~Graphics()
{
}

bool Graphics::Initialize(const int& screenWidth, const int& screenHeight, HWND hwnd)
{
	VX_LOG_ERROR("error");
	VX_LOG_WARN("warn");
	VX_LOG_INFO("info");


	bool initResult;

	m_D3D = new D3D;
	initResult = m_D3D->Initialize(screenWidth, screenHeight, hwnd, VSYNC_ENABLED, FULL_SCREEN, SCREEN_FAR, SCREEN_NEAR);
	if (!initResult) return false;

	m_debugCam = new Camera;
	m_debugCam->SetPosition({0.0f, 5.0f, -10.0f});
	// We can temporarily call Render() here since camera's position and rotation isn't changing for now
	m_debugCam->ConstructMatrix();

	// Create the shader class object
	m_shader = new DefaultBlockShader;
	m_shader->CreateObjects(m_D3D->GetDevice(), L"./Shaders/DefaultBlock_VS.hlsl", L"./Shaders/DefaultBlock_PS.hlsl");

	// Create and initialize the texture manager
	m_textureManager = new TextureManager;
	m_textureManager->Init(m_D3D->GetDevice());
	
	// Create and initialize the ImGuiLayer
	m_imGuiLayer = new ImGuiLayer;
	m_imGuiLayer->Initialize(hwnd, m_D3D->GetDevice(), m_D3D->GetDeviceContext());

	return true;
}

void Graphics::Shutdown()
{
	if(m_imGuiLayer)
	{
		m_imGuiLayer->Shutdown();
		delete m_imGuiLayer;
		m_imGuiLayer = nullptr;
	}

	if(m_textureManager)
	{
		m_textureManager->Shutdown();
		delete m_textureManager;
		m_textureManager = nullptr;
	}

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

bool Graphics::Frame(const float dt)
{
	// Begin the ImGui frame
	m_imGuiLayer->BeginFrame();

	// Begin the D3D scene
	m_D3D->BeginScene(XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));

	XMFLOAT3 cameraRot = m_debugCam->GetRotation();
	m_debugCam->SetRotation({ 25.0f, 0.0f, 0.0f });
	m_debugCam->ConstructMatrix();

	// Render models, calculate shadows, render UI, etc
	m_shader->Render(m_D3D->GetDeviceContext(), 36, m_D3D->GetWorldMatrix(), m_debugCam->GetViewMatrix(),
		m_D3D->GetProjectionMatrix(), { 1.0f, -1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, m_textureManager->GetTexture(std::string("SEAFLOOR_TEX")));


	// End the ImGui frame
	m_D3D->ClearDepthBuffer(1.0f); // Clear the depth buffer so GUI draws on top of everything
	
	m_imGuiLayer->EndFrame();

	// End the scene and present the swap chain
	m_D3D->EndScene();

	return true;
}

bool Graphics::WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	bool result;

	// Call the ImGuiLayer WndProc
	if (m_imGuiLayer->WndProc(hwnd, msg, wparam, lparam)) return true;

	// Call the D3D WndProc
	if (m_D3D->WndProc(hwnd, msg, wparam, lparam)) return true;

	return false;
	
}
