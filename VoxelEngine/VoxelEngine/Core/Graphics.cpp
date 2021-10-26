#include "../Misc/pch.h"

#include "Graphics.h"
#include "../Utility/Utility.h"

#include "../Utility/Input.h"

#include "ChunkManager.h"
#include "ChunkBufferManager.h"
#include "FrustumCulling.h"

#include "../Utility/ImGuiLayer.h"

using namespace DirectX;


bool Graphics::Initialize(const int& screenWidth, const int& screenHeight, HWND hwnd, Player* player)
{
	bool initResult;

	int32_t rtWidth, rtHeight;
	initResult = D3D::Initialize(&rtWidth, &rtHeight, hwnd, VSYNC_ENABLED, FULL_SCREEN, SCREEN_FAR, SCREEN_NEAR);
	if (!initResult) return false;

	// Set internal player pointer to player parameter
	m_player = player;

	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	m_frustumCam = new FrustumCamera();
	m_frustumCam->ConstructMatrix({ -15.0f, 20.0f, 15.0f });
	FrustumCulling::CalculateFrustum(XM_PIDIV4, (float)screenWidth / screenHeight,
		SCREEN_NEAR, SCREEN_FAR, player->GetCamera()->GetWorldMatrix(), player->GetPosition());

	// Create and initialize the texture manager
	m_textureManager = new TextureManager();
	m_textureManager->Init(D3D::GetDevice());
	
	// Initialize ChunkManager class (DefaultBlockShader will use it's data so initialization has to take place before it)
	ChunkManager::Initialize(m_player->GetPosition());
	// Initialize ChunkBufferManager class
	ChunkBufferManager::Initialize();

	// Create the shadow shader class
	m_shadowShader = new ShadowShader();
	m_shadowShader->CreateObjects(L"./Shaders/ShadowMap_VS.hlsl", L"./Shaders/ShadowMap_PS.hlsl");
	m_shadowShader->Initialize(rtWidth, rtHeight);


	// Create the chunk shader class object
	m_chunkShader = new DefaultBlockShader();
	m_chunkShader->CreateObjects(L"./Shaders/DefaultBlock_VS.hlsl", L"./Shaders/DefaultBlock_GS.hlsl", L"./Shaders/DefaultBlock_PS.hlsl");
	m_chunkShader->Initialize(m_player->GetCamera()->GetViewMatrix(), m_shadowShader->GetLightViewMatrix());

	// Create the debug renderer class object
	m_debugShader = new DebugRendererShader();
	m_debugShader->CreateObjects(L"./Shaders/DebugRenderer_VS.hlsl", L"./Shaders/DebugRenderer_PS.hlsl");
	m_debugShader->Initialize(m_player->GetCamera()->GetViewMatrix());
	

	// Create and initialize the ImGuiLayer
	m_imGuiLayer = new ImGuiLayer();
	m_imGuiLayer->Initialize(hwnd, D3D::GetDevice(), D3D::GetDeviceContext());

	m_texViewer = new TextureViewer(nullptr, 5, 5, 0.15f);

	return true;
}

void Graphics::Shutdown()
{
	ChunkManager::Shutdown();
	ChunkBufferManager::Shutdown();

	if(m_player)
	{
		delete m_player;
		m_player = nullptr;
	}

	if(m_frustumCam)
	{
		delete m_frustumCam;
		m_frustumCam = nullptr;
	}

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
}

bool Graphics::Frame(const float dt)
{
	VX_PROFILE_OUT(&GraphicsTimer_Data::frameTimer);


	// Begin the ImGui frame
	m_imGuiLayer->BeginFrame();

	ImGui::Begin("Timing Panel");
	ImGui::Text("FRAMERATE: %2.2f FPS", 1.0f / dt);
	ImGui::End();

	// Check to toggle wireframe state
	if (Input::IsKeyDown(KeyCode::E)) D3D::SetWireframeRasterState(true);
	else if (Input::IsKeyDown(KeyCode::R)) D3D::SetWireframeRasterState(false);

	// Update the debug camera's position
	m_frustumCam->Update(dt);

	FrustumCulling::CalculateFrustum(XM_PIDIV4, (float)m_screenWidth / m_screenHeight, 
		SCREEN_NEAR, SCREEN_FAR, m_player->GetCamera()->GetWorldMatrix(), m_player->GetPosition());


	//
	// !	DEBUG ONLY
	//
	// FrustumCulling::Debug_DrawFrustum();
	// auto chunkVec = ChunkManager::GetChunkVector();
	// for (auto chunk : chunkVec) FrustumCulling::Debug_DrawAABB(FrustumCulling::ConvertChunkPosToAABB(ChunkManager::ChunkToWorldSpace(chunk->GetPosition())));


	// Update the position for the updater thread
	ChunkManager::SetPlayerPos(m_player->GetPosition());

	{
		VX_PROFILE_SCOPE("[UPDATE] Chunk Buffer Update");
		// Update the vertices
		ChunkBufferManager::UpdateBuffers();
	}

	// Update the day/night cycle
	DayNightCycle::Update(dt);

	// Begin the D3D scene
	D3D::BeginScene(DayNightCycle::GetSkyColor());

	{
		VX_PROFILE_SCOPE("Render Loop");

		{
			VX_PROFILE_SCOPE("[UPDATE] Update Light Matrices");
			// Update the position and color of the light/sun
			m_shadowShader->UpdateLightMatrix();
			m_chunkShader->UpdateLightMatrix();
		}

		//{
		//	VX_PROFILE_SCOPE("[RENDER] Shadow Pass");
		//	// Render the shadow map
		//	m_shadowShader->Render();
		//}

		ID3D11ShaderResourceView* srvs[2];
		{
			VX_PROFILE_SCOPE("[SRV] Settings SRVs");
			m_texViewer->SetTexture(m_shadowShader->GetShadowMap());


			srvs[0] = m_textureManager->GetTexture(std::string("TEXTUREATLAS_TEX"));
			srvs[1] = m_shadowShader->GetShadowMap();
				
		}


		{
			VX_PROFILE_SCOPE("[RENDER] Chunk");
			// Send the chunks to the shader and render
			m_chunkShader->UpdateViewMatrices(m_player->GetCamera()->GetViewMatrix(), m_shadowShader->GetLightViewMatrix());
			m_chunkShader->Render(srvs);
		}

		{
			VX_PROFILE_SCOPE("[RENDER] Debug Lines");
			// Render all debug lines and spheres
			//m_debugShader->UpdateViewMatrix(m_player->GetCamera()->GetViewMatrix());
			//m_debugShader->Render();
		}

		{
			VX_PROFILE_SCOPE("[RENDER] Texture Viewer");
			// Render the texture viewer quad
			m_texViewer->Render();
		}
	}

	Renderer_Data::playerPos = m_player->GetPosition();
	Renderer_Data::playerPosChunkSpace = ChunkManager::WorldToChunkSpace(m_player->GetPosition());
	Renderer_Data::numActiveChunks = ChunkManager::GetNumActiveChunks();

	// End the ImGui frame
	D3D::ClearDepthBuffer(1.0f); // Clear the depth buffer so GUI draws on top of everything
	m_imGuiLayer->Draw();
	m_imGuiLayer->EndFrame();

	{
		VX_PROFILE_OUT(&GraphicsTimer_Data::presentTimer);
		// End the scene and present the swap chain
		D3D::EndScene();
	}

	return true;
}

bool Graphics::WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{

	// Call the ImGuiLayer WndProc
	if (m_imGuiLayer->WndProc(hwnd, msg, wparam, lparam)) return true;

	// Call the D3D WndProc
	if (D3D::WndProc(hwnd, msg, wparam, lparam)) return true;

	return false;
	
}
