#ifndef _GRAPHICS_H
#define _GRAPHICS_H

// TODO: Include CAMERA_CLASS, SHADERS, and other necessary classes used for rendering
#include "D3D.h"
#include "FrustumCamera.h"

#include "../Core/TextureManager.h"
#include "../Utility/ImGuiLayer.h"
#include "TextureViewer.h"

#include "DayNightCycle.h"

#include "Player.h"

// idk why it makes me include this here..again
#include <windows.h>

/////////////
// GLOBALS
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = false;
const float SCREEN_FAR = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class DefaultBlockShader;
class ShadowShader;
class DebugRendererShader;
class QuadShader;

class Graphics
{
public:

	Graphics() = default;
	Graphics(const Graphics&) = default;
	~Graphics() = default;

	bool Initialize(const int& screenWidth, const int& screenHeight, HWND windowHandle);

	void Shutdown();

	bool Frame(const float deltaTime);

	bool WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:

	FrustumCamera* m_frustumCam = nullptr;

	DefaultBlockShader* m_chunkShader = nullptr;
	ShadowShader* m_shadowShader = nullptr;
	DebugRendererShader* m_debugShader = nullptr;
	QuadShader* m_quadShader = nullptr;

	TextureManager* m_textureManager = nullptr;

	ImGuiLayer* m_imGuiLayer = nullptr;

	TextureViewer* m_texViewer = nullptr;

	// Temporary
	int m_screenWidth, m_screenHeight;
};

#endif

