#include "../Misc/pch.h"

#include "TextureManager.h"

std::map<std::string, ID3D11ShaderResourceView*> TextureManager::m_textures = std::map<std::string, ID3D11ShaderResourceView*>();

void TextureManager::Init(ID3D11Device* device)
{
	HRESULT hr;
	// TODO: Initialize all textures and add them to the map
	ID3D11ShaderResourceView* seafloorTex = nullptr;
	CreateTextureAndAddToMap(device, L"./Assets/Textures/VETextureAtlas.dds", seafloorTex, std::string("TEXTUREATLAS_TEX"));
	
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
	if (FAILED(hr)) return;
	
	// Add the SRV into the map using the provided filename
	/// NOTE!
	// If a texture with the same name is already in there release that texture
	// and insert the new one into the map, otherwise memory will leak
	if (m_textures[filename]) m_textures[filename]->Release(); 
	m_textures[filename] = out_srv;
}
