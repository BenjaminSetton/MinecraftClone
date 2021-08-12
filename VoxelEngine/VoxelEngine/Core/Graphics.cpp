#include "../Misc/pch.h"

#include "Graphics.h"
#include "../Utility/Utility.h"

#include "../Utility/Input.h"

#include "ChunkManager.h"

using namespace DirectX;

bool Graphics::Initialize(const int& screenWidth, const int& screenHeight, HWND hwnd)
{

	bool initResult;

	m_D3D = new D3D;
	initResult = m_D3D->Initialize(screenWidth, screenHeight, hwnd, VSYNC_ENABLED, FULL_SCREEN, SCREEN_FAR, SCREEN_NEAR);
	if (!initResult) return false;

	m_debugCam = new DebugCamera;
	m_debugCam->SetPosition({-4.0f, 14.0f, -10.0f});
	m_debugCam->ConstructMatrix();


	// Create and initialize the texture manager
	m_textureManager = new TextureManager;
	m_textureManager->Init(m_D3D->GetDevice());
	
	// Create the shader class object
	m_chunkShader = new DefaultBlockShader;
	m_chunkShader->CreateObjects(m_D3D->GetDevice(), L"./Shaders/DefaultBlock_VS.hlsl", L"./Shaders/DefaultBlock_PS.hlsl");
	m_chunkShader->Initialize(m_D3D->GetDeviceContext(), m_D3D->GetWorldMatrix(), m_debugCam->GetViewMatrix(),
		m_D3D->GetProjectionMatrix(), { 1.0f, -1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, m_textureManager->GetTexture(std::string("SEAFLOOR_TEX")));
	
	// Create and initialize the ImGuiLayer
	m_imGuiLayer = new ImGuiLayer;
	m_imGuiLayer->Initialize(hwnd, m_D3D->GetDevice(), m_D3D->GetDeviceContext());

	ChunkManager::Initialize(m_debugCam->GetPosition());

	return true;
}

void Graphics::Shutdown()
{
	ChunkManager::Shutdown();

	if(m_imGuiLayer)
	{
		m_imGuiLayer->Shutdown();
		delete m_imGuiLayer;
		m_imGuiLayer = nullptr;
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
	// Debugging stats
	int numDrawCalls = 0;
	int numVertices = 0;
	int numChunks = 0;

	// Begin the ImGui frame
	m_imGuiLayer->BeginFrame();

	{
		VX_PROFILE_FUNC();

		// Update the debug camera's position
		m_debugCam->Update(dt);

		// Update the active chunks
		ChunkManager::Update(m_debugCam->GetPosition());

		// Check to toggle wireframe state
		if (Input::IsKeyDown(KeyCode::E)) m_D3D->SetWireframeRasterState(true);
		else if (Input::IsKeyDown(KeyCode::R)) m_D3D->SetWireframeRasterState(false);

		// Begin the D3D scene
		m_D3D->BeginScene(XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));

		{
			VX_PROFILE_SCOPE("Render Loop");
			// Send the current chunk to the shader and render
			for (uint16_t i = 0; i < ChunkManager::GetNumActiveChunks(); i++)
			{
				// Render the chunks
				m_chunkShader->SetChunk(ChunkManager::GetChunkAtIndex(i));
				m_chunkShader->UpdateViewMatrix(m_D3D->GetDeviceContext(), m_debugCam->GetViewMatrix());
				m_chunkShader->Render(m_D3D->GetDeviceContext());

				numChunks++;
				numDrawCalls++;
				numVertices += ChunkManager::GetChunkAtIndex(i)->GetNumFaces() * 4;
			}
		}


		ImGui::Begin("Debug Panel");
		ImGui::Text("Chunk Count: %i", numChunks);
		ImGui::Text("Draw Calls: %i", numDrawCalls);
		ImGui::Text("Vertex Count: %i", numVertices);
		ImGui::End();

	}

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
