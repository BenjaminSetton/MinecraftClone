#ifndef _TEXTUREMANAGER_H
#define _TEXTUREMANAGER_H

#include <d3d11.h>
#include "../Utility/DDSTextureLoader.h"
#include <map>

class TextureManager
{
public:

	TextureManager();
	TextureManager(const TextureManager& texManager);
	~TextureManager();

	static void Init(ID3D11Device* device);

	static void Shutdown();

	static ID3D11ShaderResourceView* GetTexture(const std::string& texName);

private:

	static void CreateTextureAndAddToMap(ID3D11Device* device, const WCHAR* filepath, 
		ID3D11ShaderResourceView* out_srv, const std::string& filename);

	static std::map<std::string, ID3D11ShaderResourceView*> m_textures;
};

#endif