#include "../Misc/pch.h"

#include "Application.h"
#include "../Utility/FileSystem/FileSystem.h"
#include "Game.h"
#include "Graphics.h"
#include "../Utility/HeapOverrides.h"
#include "../Utility/Utility.h"
#include "../Utility/MathTypes.h"

#include "../Utility/Input.h"

#include "DefaultBlockShader.h"
#include "ShadowShader.h"
#include "DebugRendererShader.h"
#include "QuadShader.h"

#include "ChunkManager.h"
#include "ShaderBufferManagers/ChunkBufferManager.h"
#include "ShaderBufferManagers/QuadBufferManager.h"
#include "ShaderBufferManagers/QuadNDCBufferManager.h"
#include "FrustumCulling.h"
#include "Crosshair.h"
#include "UI/UIRenderer.h"

#include "../Utility/Math.h"

using namespace DirectX;

namespace Orange
{
	bool Graphics::Initialize()
	{
		bool initResult;

		initResult = D3D::Initialize(VSYNC_ENABLED, SCREEN_FAR, SCREEN_NEAR);
		if (!initResult) return false;

		Vec2 screenDimensions = Application::Handle->GetMainWindow()->GetSize();
		m_screenWidth = static_cast<int>(screenDimensions.x);
		m_screenHeight = static_cast<int>(screenDimensions.y);

		Player* player = Game::GetPrimaryPlayer();

		m_frustumCam = OG_NEW FrustumCamera();
		m_frustumCam->ConstructMatrix({ -15.0f, 20.0f, 15.0f }, { 0.0f, 0.0f, 0.0f });
		FrustumCulling::CalculateFrustum(XM_PIDIV4, (float)m_screenWidth / m_screenHeight,
			SCREEN_NEAR, SCREEN_FAR, player->GetCamera(CameraType::FirstPerson)->GetWorldMatrix(), player->GetPosition());

		// Create and initialize the texture manager
		m_textureManager = OG_NEW TextureManager();
		m_textureManager->Init(D3D::GetDevice());
	
		// Initialize ChunkManager class (DefaultBlockShader will use it's data so initialization has to take place before it)
		ChunkManager::Initialize(player->GetPosition());

		// Initialize ChunkBufferManager class
		ChunkBufferManager::Initialize();
		QuadBufferManager::Initialize();
		QuadNDCBufferManager::Initialize();

		UIRenderer::Initialize();

		// Create the chunk shader class object
		m_chunkShader = OG_NEW DefaultBlockShader();

		m_chunkShader->CreateObjects
		(
			FileSystem::GetFileNameRelativeToGeneratedDirectory(L"DefaultBlock_VS.hlsl").c_str(),
			FileSystem::GetFileNameRelativeToGeneratedDirectory(L"DefaultBlock_GS.hlsl").c_str(),
			FileSystem::GetFileNameRelativeToGeneratedDirectory(L"DefaultBlock_PS.hlsl").c_str()
		);
		m_chunkShader->Initialize(player->GetCamera(CameraType::FirstPerson)->GetViewMatrix());

		// Create the debug renderer class object
		m_debugShader = OG_NEW DebugRendererShader();
		m_debugShader->CreateObjects
		(
			FileSystem::GetFileNameRelativeToGeneratedDirectory(L"DebugRenderer_VS.hlsl").c_str(),
			FileSystem::GetFileNameRelativeToGeneratedDirectory(L"DebugRenderer_PS.hlsl").c_str()
		);
		m_debugShader->Initialize(player->GetCamera(CameraType::FirstPerson)->GetViewMatrix());

		// Create the quad shader class object
		m_quadShader = OG_NEW QuadShader();
		m_quadShader->CreateObjects
		(
			FileSystem::GetFileNameRelativeToGeneratedDirectory(L"Quad_VS.hlsl").c_str(),
			FileSystem::GetFileNameRelativeToGeneratedDirectory(L"NDCQuad_VS.hlsl").c_str(),
			FileSystem::GetFileNameRelativeToGeneratedDirectory(L"Quad_PS.hlsl").c_str());
		m_quadShader->Initialize();
	
		// Initialize the ImGuiLayer
		//ImGuiLayer::Initialize(Application::Handle->GetMainWindow()->GetHWND(), D3D::GetDevice(), D3D::GetDeviceContext());

		m_texViewer = OG_NEW TextureViewer(nullptr, 5, 5, 0.15f);

		return true;
	}

	void Graphics::Shutdown()
	{
		ChunkManager::Shutdown();
		ChunkBufferManager::Shutdown();
		QuadBufferManager::Shutdown();
		QuadNDCBufferManager::Shutdown();
		UIRenderer::Deinitialize();

		if (m_quadShader)
		{
			m_quadShader->Shutdown();
			delete m_quadShader;
			m_quadShader = nullptr;
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

		if(m_debugShader)
		{
			m_debugShader->Shutdown();
			delete m_debugShader;
			m_debugShader = nullptr;
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
		OG_PROFILE_OUT(&GraphicsTimer_Data::frameTimer);

		Player* player = Game::GetPrimaryPlayer();

		/*ImGui::Begin("Timing Panel");
		ImGui::Text("FRAMERATE: %2.2f FPS", 1.0f / dt);
		ImGui::End();*/

		// Check to toggle wireframe state
		if (Input::IsKeyDown(KeyCode::E)) D3D::SetWireframeRasterState(true);
		else if (Input::IsKeyDown(KeyCode::R)) D3D::SetWireframeRasterState(false);

		// Update the debug camera's position
		m_frustumCam->Update(dt);

		Crosshair::Update(dt);

		FrustumCulling::CalculateFrustum(XM_PIDIV4, (float)m_screenWidth / m_screenHeight, 
			SCREEN_NEAR, SCREEN_FAR, player->GetCamera(CameraType::FirstPerson)->GetWorldMatrix(), player->GetPosition());


		// Update the position for the updater thread
		ChunkManager::SetPlayerPos(player->GetPosition());

		{
			OG_PROFILE_SCOPE("[UPDATE] Chunk Buffer Update");
			// Update the vertices
			ChunkBufferManager::UpdateBuffers();
		}

		// Update the day/night cycle
		DayNightCycle::Update(dt);

		// Decrement the clear timer for debug objects
		if(DebugRenderer::GetVertexCount() > 0) DebugRenderer::DecreaseCurrentClearTimer(dt);

		// Begin the D3D scene
		D3D::BeginScene(DayNightCycle::GetSkyColor());

		{
			OG_PROFILE_SCOPE("Render Loop");

			{
				OG_PROFILE_SCOPE("[UPDATE] Update Light Matrices");
				// Update the position and color of the light/sun
				m_chunkShader->UpdateLightMatrix();
			}

			//{
			//	OG_PROFILE_SCOPE("[RENDER] Shadow Pass");
			//	// Render the shadow map
			//	m_shadowShader->Render();
			//}

			ID3D11ShaderResourceView* srvs[1];
			{
				OG_PROFILE_SCOPE("[SRV] Settings SRVs");
				srvs[0] = m_textureManager->GetTexture(std::string("TEXTUREATLAS_TEX"));
			}

			{
				OG_PROFILE_SCOPE("[RENDER] Chunk");
				// Send the chunks to the shader and render
				m_chunkShader->UpdateViewMatrices(player->GetCamera(player->GetSelectedCameraType())->GetViewMatrix(), XMMatrixIdentity());
				m_chunkShader->Render(srvs);
			}

			{
				OG_PROFILE_SCOPE("[RENDER] Debug Lines");
				// Render all debug lines and spheres
				m_debugShader->UpdateViewMatrix(player->GetCamera(player->GetSelectedCameraType())->GetViewMatrix());
				m_debugShader->Render();
			}

			{
				D3D::ClearDepthBuffer(1.0f);
				OG_PROFILE_SCOPE("[RENDER] Quads");
				m_quadShader->UpdateViewMatrix(player->GetCamera(player->GetSelectedCameraType())->GetViewMatrix());
				m_quadShader->SetQuadTexture(m_textureManager->GetTexture(std::string("BLOCKSELECTOR_TEX")));
				m_quadShader->SetRenderInNDC(false);
				m_quadShader->Render();

				m_quadShader->SetQuadTexture(m_textureManager->GetTexture(std::string("CROSSHAIR_TEX")));
				m_quadShader->SetRenderInNDC(true);
				m_quadShader->Render();

				UIRenderer::Draw();
			}

			{
				//OG_PROFILE_SCOPE("[RENDER] Texture Viewer");
				//// Render the texture viewer quad
				//m_texViewer->Render();
			}

		}

		Renderer_Data::playerPos = player->GetPosition();
		Renderer_Data::playerPosChunkSpace = Orange::Math::WorldToChunkSpace(player->GetPosition());
		Renderer_Data::numActiveChunks = ChunkManager::GetNumActiveChunks();

		//D3D::ClearDepthBuffer(1.0f); // Clear the depth buffer so GUI draws on top of everything

		return true;
	}

	void Graphics::Present()
	{
		OG_PROFILE_OUT(&GraphicsTimer_Data::presentTimer);
		// End the scene and present the swap chain
		D3D::EndScene();
	}

	bool Graphics::WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		// Call the ImGuiLayer WndProc
		//if (ImGuiLayer::WndProc(hwnd, msg, wparam, lparam)) return true;

		// Call the D3D WndProc
		if (D3D::WndProc(hwnd, msg, wparam, lparam)) return true;

		return false;
	}
}


