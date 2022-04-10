#include "../Misc/pch.h"

#include "Texture.h"

const TextureFormat Texture::GetFormat() { return m_format; }

void* Texture::GetData() const { return m_data; }