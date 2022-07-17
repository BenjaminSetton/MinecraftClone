#ifndef _TEXTBOX_H
#define _TEXTBOX_H

#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <string>
#include <unordered_map>

#include "../../Utility/MathTypes.h"
#include "UIElement.h"

namespace Orange
{

	class TextBox : public UIElement
	{
	public:

		const bool operator==(const TextBox& other)
		{
			if
			(
				m_text == other.m_text &&
				m_position == other.m_position &&
				m_size == other.m_size &&
				m_color == other.m_color
			)
			{
				return true;
			}

			return false;
		}

		TextBox();
		TextBox(const std::string text, const Vec2 position = Vec2(50,50), const Vec2 size = Vec2(200, 200), const Vec2 padding = Vec2(10, 10), const Vec3 color = Vec3(1, 1, 1));
		TextBox(const TextBox& other);
		~TextBox();

		const std::string& GetText();
		void SetText(const std::string text);
		void AppendText(const std::string text);
		void ClearText();
		void SetDrawOutline(const bool drawOutline);

		const UIElementType GetType() const override;
		void Draw() const override;

		//friend class TextBoxRenderer;

	private:

		std::string m_text;
		bool m_drawOutline;

	};

	//class TextBoxRenderer
	//{
	//public:

	//	static void Initialize();
	//	static void Deinitialize();

	//	static void Draw(const TextBox* textBox);

	//private:

	//	static void CreateObjects();
	//	static void CreateShaders();

	//	// Bind once per frame
	//	static void BindVertexBuffer();
	//	static void BindObjects();

	//	// Update multiple times per frame for every character
	//	static void BindCharTexture(const char c);

	//	static void DeleteObjects();
	//	static void DeleteShaders();

	//	static void CreateCharSRVAndAddToMap(const char c);

	//private:

	//	static UIVertex m_UIVertices[4];
	//	static std::unordered_map<char, ID3D11ShaderResourceView*> m_charToSRVMap;

	//	// Stores a transposed orthographic projection matrix, which basically maps any vertex position from
	//	// ( [0, screenWidth], [0, screenHeight] ) -> ( [0, 1], [0, 1] )
	//	static DirectX::XMMATRIX m_orthographicProjection;

	//	/////////////////////////////////////////
	//	// 
	//	// RENDERER API OBJECTS
	//	//
	//	/////////////////////////////////////////

	//	static ID3D11VertexShader* m_vertexShader;
	//	static ID3D11PixelShader* m_pixelShader;
	//	static ID3D11Buffer* m_constantBuffer;
	//	static ID3D11Buffer* m_vertexBuffer;
	//	static ID3D11InputLayout* m_inputLayout;
	//	static ID3D11SamplerState* m_samplerClamp;
	//	static ID3D11BlendState* m_blendState;

	//};
}


#endif