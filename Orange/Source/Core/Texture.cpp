#include "../Misc/pch.h"

#include "Texture.h"
#include "../Utility/Utility.h"

namespace Orange
{

	Texture::Texture(const TextureData& data)
	{
		// Performs shallow copy; Create() will create the deep copy of the data
		m_texData = data;

		Create();
	}

	Texture::Texture(const Texture& other)
	{
		// Performs shallow copy; Create() will create the deep copy of the data
		m_texData = other.m_texData;

		Create();
	}

	Texture::~Texture()
	{
		if (!m_isReference)
		{
			Destroy();
		}
	}

	const TextureData& Texture::GetTextureData() const
	{
		return m_texData;
	}

	void Texture::SetTextureData(const TextureData& data)
	{
		// Destroy any old data before creating a new texture
		Destroy();

		// Shallow copy
		m_texData = data;

		// Deep copy of data
		Create();

		m_isReference = false;
	}


	void Texture::SetReferenceDataToTexture(const TextureData& data)
	{
		// Shallow copy
		m_texData = data;

		m_isReference = true;
	}

	// All the data should be set at this point. We just need to copy
	// the data over and set our pointer to point to the new data
	void Texture::Create()
	{
		// This is 64 bits because a 32-bit uint would not be able to hold
		// a 4K texture with a 32-bit unit size
		uint64_t dataSize = 0;
		switch (m_texData.dimensions)
		{
		case TextureDimensions::INVALID:
		{
			OG_ASSERT_MSG(false, "How did we even get here");
			return;
		}
		case TextureDimensions::ONE:
		{
			if (m_texData.size.x > 0 && m_texData.size.y <= 0 && m_texData.size.z <= 0)
			{
				dataSize = static_cast<uint64_t>(m_texData.size.x);
			}
			else
			{
				//OG_ASSERT_MSG(false, "Creating a 1D texture with an invalid size. Aborting texture creation");

				// We will allow generating textures with no width and height, but I should find a way to throw a
				// warnings on the runtime logs just so the user is aware
				break;
			}
			break;
		}
		case TextureDimensions::TWO:
		{
			if (m_texData.size.x > 0 && m_texData.size.y > 0 && m_texData.size.z <= 0)
			{
				dataSize = static_cast<uint64_t>(m_texData.size.x) * static_cast<uint64_t>(m_texData.size.y);
			}
			else
			{
				//OG_ASSERT_MSG(false, "Creating a 2D texture with an invalid size. Aborting texture creation");
				
				// We will allow generating textures with no width and height, but I should find a way to throw a
				// warnings on the runtime logs just so the user is aware
				break;
			}
			break;
		}
		case TextureDimensions::THREE:
		{
			if (m_texData.size.x > 0 && m_texData.size.y > 0 && m_texData.size.z > 0)
			{
				dataSize = static_cast<uint64_t>(m_texData.size.x) * static_cast<uint64_t>(m_texData.size.y) * static_cast<uint64_t>(m_texData.size.z);
			}
			else
			{
				//OG_ASSERT_MSG(false, "Creating a 3D texture with an invalid size. Aborting texture creation");
				
				// We will allow generating textures with no width and height, but I should find a way to throw a
				// warnings on the runtime logs just so the user is aware
				break;
			}
			break;
		}
		}

		if (dataSize > 0)
		{
			char* data = new char[dataSize];
			memcpy(static_cast<void*>(data), m_texData.data, dataSize * static_cast<uint64_t>(GetUnitSize()));
			m_texData.data = data;
		}
		else // dataSize <= 0
		{
			// In cases where the texture has no width and height, we don't want to keep
			// the old data pointer laying around
			m_texData.data = nullptr;
		}
	}

	void Texture::Destroy()
	{
		m_texData.format = TextureFormat::INVALID;
		m_texData.dimensions = TextureDimensions::INVALID;
		m_texData.size = Vec3(0.0f);

		delete[] m_texData.data;
	}

	uint32_t Texture::GetUnitSize() 
	{
		uint32_t unitSize = 0;
		switch (m_texData.format)
		{
		case TextureFormat::INVALID:
		{
			OG_ASSERT_MSG(false, "Attempting to retrieve the unit size of a texture of INVALID format.\nMaybe forgot to add case for new enum value");
			break;
		}
		case TextureFormat::RGBA_32:
		{
			unitSize = 4;
			break;
		}
		case TextureFormat::R_8:
		{
			unitSize = 1;
			break;
		}
		}

		return unitSize;
	}

}

