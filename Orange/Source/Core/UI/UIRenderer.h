#ifndef _UIRENDERER_H
#define _UIRENDERER_H

#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <queue>
#include <unordered_map>

namespace Orange
{

	// Forward declarations
	class Texture;
	struct UIDrawCommand;
	struct UIVertex;

	class UIRenderer
	{
	public:

		UIRenderer() = default;
		UIRenderer(const UIRenderer& other) = delete;
		~UIRenderer() = default;

		static void Initialize();

		static void Deinitialize();

		static void Draw();

	private:

		// Helper draw functions specific to each UIElement
		static void DrawChar(const UIDrawCommand& drawCommand, const UIVertex* vertices);
		static void DrawImage(const UIDrawCommand& drawCommand, const UIVertex* vertices);
		static void DrawContainer(const UIDrawCommand& drawCommand, const UIVertex* vertices);

		static void CreateObjects();
		static void CreateShaders();

		// Bind once per frame
		static void BindVertexBuffer();
		static void BindObjects();

		// Update multiple times per frame for every character
		//static void BindCharTexture(const char c);
		static void BindTexture(const uint64_t id);

		static void DeleteObjects();
		static void DeleteShaders();

		//static void CreateCharSRVAndAddToMap(const Texture& tex, const char c);
		static void CreateSRVFromTexture(const Texture& tex);

	private:

		static UIVertex m_UIVertices[4];
		static std::unordered_map<uint64_t, ID3D11ShaderResourceView*> m_idToSRVMap;

		// Stores a transposed orthographic projection matrix, which basically maps any vertex position from
		// ( [0, screenWidth], [0, screenHeight] ) -> ( [0, 1], [0, 1] )
		static DirectX::XMMATRIX m_orthographicProjection;

		/////////////////////////////////////////
		// 
		// RENDERER API OBJECTS
		//
		/////////////////////////////////////////

		static ID3D11VertexShader* m_vertexShader;
		static ID3D11PixelShader* m_pixelShader;
		static ID3D11Buffer* m_constantBuffer;
		static ID3D11Buffer* m_vertexBuffer;
		static ID3D11InputLayout* m_inputLayout;
		static ID3D11SamplerState* m_samplerClamp;
		static ID3D11BlendState* m_blendState;
		static ID3D11RasterizerState* m_rasterState;

	};

}

#endif