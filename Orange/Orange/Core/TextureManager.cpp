#include "../Misc/pch.h"

#include "TextureManager.h"
#include "../Utility/Utility.h"

std::map<std::string, ID3D11ShaderResourceView*> TextureManager::m_textures = std::map<std::string, ID3D11ShaderResourceView*>();

void TextureManager::Init(ID3D11Device* device)
{
	// TODO: Initialize all textures and add them to the map
	ID3D11ShaderResourceView* seafloorTex = nullptr;
	CreateTextureAndAddToMap(device, L"./Assets/Textures/OGTextureAtlas.dds", seafloorTex, std::string("TEXTUREATLAS_TEX"));
	
	// Block Selector UI
	ID3D11ShaderResourceView* blockSelectorTex = nullptr;
	CreateTextureAndAddToMap(device, L"./Assets/Textures/BlockSelectorUI.dds", blockSelectorTex, std::string("BLOCKSELECTOR_TEX"));

	// Crosshair
	ID3D11ShaderResourceView* crosshairTex = nullptr;
	CreateTextureAndAddToMap(device, L"./Assets/Textures/Crosshair.dds", crosshairTex, std::string("CROSSHAIR_TEX"));
	
}

void TextureManager::Shutdown()
{
	for (auto tex : m_textures)
	{
		if(tex.second)
			tex.second->Release();
	}
	
	m_textures.clear();
}

ID3D11ShaderResourceView* TextureManager::GetTexture(const std::string& texName)
{
	return m_textures[texName];
}

void TextureManager::CreateTextureAndAddToMap(ID3D11Device* device, const WCHAR* filepath, 
	ID3D11ShaderResourceView* out_srv, const std::string& filename)
{
	HRESULT hr;
	// Create the texture from DDS file and store in SRV
	hr = DirectX::CreateDDSTextureFromFile(device, filepath, nullptr, &out_srv);
	if (FAILED(hr))
	{
		OG_ASSERT_MSG(false, "Unable to create texture from DDS file!");
		return;
	}
	
	// Add the SRV into the map using the provided filename
	/// NOTE!
	// If a texture with the same name is already in there release that texture
	// and insert the new one into the map, otherwise memory will leak
	if (m_textures[filename]) m_textures[filename]->Release(); 
	m_textures[filename] = out_srv;
}
