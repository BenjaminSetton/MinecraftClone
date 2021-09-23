#include "../Misc/pch.h"

#include "Graphics.h"
#include "../Utility/Utility.h"

#include "../Utility/Input.h"

#include "ChunkManager.h"

using namespace DirectX;


bool Graphics::Initialize(const int& screenWidth, const int& screenHeight, HWND hwnd)
{

	bool initResult;

	initResult = D3D::Initialize(screenWidth, screenHeight, hwnd, VSYNC_ENABLED, FULL_SCREEN, SCREEN_FAR, SCREEN_NEAR);
	if (!initResult) return false;

	m_debugCam = new DebugCamera;
	m_debugCam->SetPosition({-4.0f, 30.0f, -10.0f});
	m_debugCam->ConstructMatrix();

	// Create and initialize the texture manager
	m_textureManager = new TextureManager;
	m_textureManager->Init(D3D::GetDevice());
	
	// Initialize ChunkManager class (DefaultBlockShader will use it's data so initialization has to take place before it)
	ChunkManager::Initialize(m_debugCam->GetPosition());

	// Create the shadow shader class
	m_shadowShader = new ShadowShader;
	m_shadowShader->CreateObjects(L"./Shaders/ShadowMap_VS.hlsl", L"./Shaders/ShadowMap_PS.hlsl");
	m_shadowShader->Initialize(screenWidth, screenHeight);


	// Create the chunk shader class object
	m_chunkShader = new DefaultBlockShader;
	m_chunkShader->CreateObjects(L"./Shaders/DefaultBlock_VS.hlsl", L"./Shaders/DefaultBlock_PS.hlsl");
	m_chunkShader->Initialize(m_debugCam->GetViewMatrix(), m_shadowShader->GetLightViewMatrix());

	// Create the debug renderer class object
	m_debugShader = new DebugRendererShader;
	m_debugShader->CreateObjects(L"./Shaders/DebugRenderer_VS.hlsl", L"./Shaders/DebugRenderer_PS.hlsl");
	m_debugShader->Initialize(m_debugCam->GetViewMatrix());
	

	// Create and initialize the ImGuiLayer
	m_imGuiLayer = new ImGuiLayer;
	m_imGuiLayer->Initialize(hwnd, D3D::GetDevice(), D3D::GetDeviceContext());

	m_texViewer = new TextureViewer(nullptr, 5, 5, 0.20f);

	return true;
}

void Graphics::Shutdown()
{
	ChunkManager::Shutdown();

	if(m_texViewer)
	{
		delete m_texViewer;
		m_texViewer = nullptr;
	}

	if(m_imGuiLayer)
	{
		m_imGuiLayer->Shutdown();
		delete m_imGuiLayer;
		m_imGuiLayer = nullptr;
	}

	if(m_debugShader)
	{
		m_debugShader->Shutdown();
		delete m_debugShader;
		m_debugShader = nullptr;
	}

	if(m_shadowShader)
	{
		m_shadowShader->Shutdown();
		delete m_shadowShader;
		m_shadowShader = nullptr;
	}

	if (m_chunkShader)
	{
		m_chunkShader->Shutdown();
		delete m_chunkShader;
		m_chunkShader = nullptr;
	}

	if(m_textureManager)
	{
		m_textureManager->Shutdown();
		delete m_textureManager;
		m_textureManager = nullptr;
	}

	D3D::Shutdown();

	if (m_debugCam) 
	{
		delete m_debugCam;
		m_debugCam = nullptr;
	}
}

bool Graphics::Frame(const float dt)
{
	// Debugging stats
	int numChunks = 0;

	// Begin the ImGui frame
	m_imGuiLayer->BeginFrame();

	{
		VX_PROFILE_FUNC();

		// Check to toggle wireframe state
		if (Input::IsKeyDown(KeyCode::E)) D3D::SetWireframeRasterState(true);
		else if (Input::IsKeyDown(KeyCode::R)) D3D::SetWireframeRasterState(false);

		// Update the debug camera's position
		m_debugCam->Update(dt);

		// Update the position for the updater thread
		ChunkManager::SetPlayerPos(m_debugCam->GetPosition());

		// Update the day/night cycle
		DayNightCycle::Update(dt);

		// Begin the D3D scene
		D3D::BeginScene(XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));

		{
			VX_PROFILE_SCOPE_MSG("Render Loop");

			// Update the position and color of the light/sun
			m_shadowShader->UpdateLightMatrix();
			m_chunkShader->UpdateLightMatrix();

			// Render the shadow map
			m_shadowShader->Render();

			m_texViewer->SetTexture(m_shadowShader->GetShadowMap());

			ID3D11ShaderResourceView* srvs[] =
			{
				m_textureManager->GetTexture(std::string("TEXTUREATLAS_TEX")),
				m_shadowShader->GetShadowMap()
			};
			// Send the chunks to the shader and render
			m_chunkShader->UpdateViewMatrices(m_debugCam->GetViewMatrix(), m_shadowShader->GetLightViewMatrix());
			m_chunkShader->Render(srvs);

			// Render all debug lines and spheres
			m_debugShader->UpdateViewMatrix(m_debugCam->GetViewMatrix());
			m_debugShader->Render();

			// Render the texture viewer quad
			m_texViewer->Render();
		}

		XMFLOAT3 playerPos = m_debugCam->GetPosition();
		XMFLOAT3 playerPosChunkSpace = ChunkManager::WorldToChunkSpace(playerPos);
		ImGui::Begin("Debug Panel");
		ImGui::Text("Player Position: %2.2f, %2.2f, %2.2f (%i, %i, %i)",
			playerPos.x, playerPos.y, playerPos.z,
			(int)playerPosChunkSpace.x, (int)playerPosChunkSpace.y, (int)playerPosChunkSpace.z);
		ImGui::Text("Active Chunks: %i", ChunkManager::GetNumActiveChunks());
		ImGui::Text("Render Distance: %i", 8);
		ImGui::End();
	}

	// End the ImGui frame
	D3D::ClearDepthBuffer(1.0f); // Clear the depth buffer so GUI draws on top of everything
	m_imGuiLayer->EndFrame();

	// End the scene and present the swap chain
	D3D::EndScene();

	return true;
}

bool Graphics::WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	bool result;

	// Call the ImGuiLayer WndProc
	if (m_imGuiLayer->WndProc(hwnd, msg, wparam, lparam)) return true;

	// Call the D3D WndProc
	if (D3D::WndProc(hwnd, msg, wparam, lparam)) return true;

	return false;
	
}
