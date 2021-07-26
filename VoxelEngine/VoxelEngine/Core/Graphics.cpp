#include "../Misc/pch.h"

#include "Graphics.h"
#include "../Utility/Utility.h"

using namespace DirectX;

bool Graphics::Initialize(const int& screenWidth, const int& screenHeight, HWND hwnd)
{

	bool initResult;

	m_D3D = new D3D;
	initResult = m_D3D->Initialize(screenWidth, screenHeight, hwnd, VSYNC_ENABLED, FULL_SCREEN, SCREEN_FAR, SCREEN_NEAR);
	if (!initResult) return false;

	m_debugCam = new Camera;
	m_debugCam->SetPosition({-4.0f, 1.65f, -5.0f});
	// We can temporarily call Render() here since camera's position and rotation isn't changing for now
	m_debugCam->SetRotation({ 0.0f, 0.0f, 0.0f });
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

	// Expose the camera stats to ImGui
	XMFLOAT3 camPosF3 = m_debugCam->GetPosition();
	float camPos[] = { camPosF3.x, camPosF3.y, camPosF3.z };
	XMFLOAT3 camRotF3 = m_debugCam->GetRotation();
	float camRot[] = { camRotF3.x, camRotF3.y, camRotF3.z };
	ImGui::Begin("Camera Debug");
	ImGui::SliderFloat3("Camera Pos", camPos, -20.0f, 20, "%2.2f", 1);
	ImGui::SliderFloat3("Camera Rot", camRot, -180.0f, 180.0f, "%2.2f", 1);
	ImGui::End();
	m_debugCam->SetPosition({ camPos[0], camPos[1], camPos[2] });
	m_debugCam->SetRotation({ camRot[0], camRot[1], camRot[2] });
	m_debugCam->ConstructMatrix();

	// Begin the D3D scene
	m_D3D->BeginScene(XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));

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
