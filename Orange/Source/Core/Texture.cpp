#include "../Misc/pch.h"

#include "../Utility/Math.h"
#include "Texture.h"
#include "TextureRegistry.h"
#include "../Utility/Utility.h"

namespace Orange
{

	Texture::Texture() { }

	Texture::Texture(const TextureSpecs& specs, const void* data)
	{
		// Performs shallow copy; Create() will create the deep copy of the data
		m_specs = specs;

		Create(data);
	}

	Texture::Texture(const Texture& other)
	{
		// The copy constructor will only perform a shallow copy, and increase the ref_count
		// of the data pointer as a result.
		m_specs = other.m_specs;
		m_data = other.m_data;
		m_id = other.m_id;
	}

	const TextureSpecs& Texture::GetSpecs() const
	{
		return m_specs;
	}

	Ref<void> Texture::GetData() const
	{
		return m_data;
	}

	const uint64_t& Texture::GetId() const
	{
		return m_id;
	}

	void Texture::CreateSolidColorTexture(const Vec4& color)
	{
		OG_ASSERT_MSG(m_data.get() == nullptr, "Overwriting data when attempting to create a solid color texture");

		// Set the appropriate texture data
		m_specs.dimensions = TextureDimensions::TWO;
		m_specs.format = TextureFormat::RGBA_32;
		m_specs.size = Vec3(1.0f, 1.0f, 0.0f);

		// Set the data
		uint64_t numBytes = static_cast<uint64_t>(GetUnitSize());
		uint32_t baseColor = ConvertVec4IntoRGBA32(color);
		OG_ASSERT_MSG(numBytes == 4, "Attempting to create a texture from a solid color of size 1 that's not 4 bytes?");
		AllocateAndPopulateBuffer(static_cast<void*>(&baseColor), numBytes);
	}

	const bool Texture::IsValid() const
	{
		return m_data.get() != nullptr;
	}

	// All the data should be set at this point. We just need to copy
	// the data over and set our pointer to point to the new data
	void Texture::Create(const void* data)
	{
		// This is 64 bits because a 32-bit uint would not be able to hold
		// a 4K texture with a 32-bit unit size
		uint64_t dataSize = 0;
		switch (m_specs.dimensions)
		{
		case TextureDimensions::INVALID:
		{
			OG_ASSERT_MSG(false, "How did we even get here");
			return;
		}
		case TextureDimensions::ONE:
		{
			if (m_specs.size.x > 0 && m_specs.size.y <= 0 && m_specs.size.z <= 0)
			{
				dataSize = static_cast<uint64_t>(m_specs.size.x);
			}
			else
			{
				// We will allow generating textures with no width and height, but I should find a way to throw a
				// warnings on the runtime logs just so the user is aware
				break;
			}
			break;
		}
		case TextureDimensions::TWO:
		{
			if (m_specs.size.x > 0 && m_specs.size.y > 0 && m_specs.size.z <= 0)
			{
				dataSize = static_cast<uint64_t>(m_specs.size.x) * static_cast<uint64_t>(m_specs.size.y);
			}
			else
			{
				// We will allow generating textures with no width and height, but I should find a way to throw a
				// warnings on the runtime logs just so the user is aware
				break;
			}
			break;
		}
		case TextureDimensions::THREE:
		{
			if (m_specs.size.x > 0 && m_specs.size.y > 0 && m_specs.size.z > 0)
			{
				dataSize = static_cast<uint64_t>(m_specs.size.x) * static_cast<uint64_t>(m_specs.size.y) * static_cast<uint64_t>(m_specs.size.z);
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
			uint64_t numBytes = dataSize * static_cast<uint64_t>(GetUnitSize());
			AllocateAndPopulateBuffer(data, numBytes);
		}
		else // dataSize <= 0
		{
			// In cases where the texture has no width and height, we don't want to keep
			// the old data pointer laying around
			m_data.reset();
		}
	}

	uint32_t Texture::GetUnitSize() const
	{
		uint32_t unitSize = 0;
		switch (m_specs.format)
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

	uint32_t Texture::ConvertVec4IntoRGBA32(const Vec4& color)
	{
		uint8_t r, g, b, a;
		uint32_t min, max;
		min = 0x00;
		max = 0xFF;

		r = static_cast<uint8_t>(Math::Clamp(static_cast<uint32_t>(color.x * 255.0f), min, max));
		g = static_cast<uint8_t>(Math::Clamp(static_cast<uint32_t>(color.y * 255.0f), min, max));
		b = static_cast<uint8_t>(Math::Clamp(static_cast<uint32_t>(color.z * 255.0f), min, max));
		a = static_cast<uint8_t>(Math::Clamp(static_cast<uint32_t>(color.w * 255.0f), min, max));

		uint32_t res = 0;
		res |= r   << 24;
		res |= g   << 16;
		res |= b   << 8;
		res |= a/* << 0*/;
		return res;
	}

	void Texture::AllocateAndPopulateBuffer(const void* data, const uint64_t numBytes)
	{
		m_data = std::make_shared<char[]>(numBytes);
		memcpy(static_cast<void*>(m_data.get()), data, numBytes);

		uint64_t hash = TextureRegistry::AddTextureToRegistry(*this);
		m_id = hash;
	}

}

