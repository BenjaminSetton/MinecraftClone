#ifndef _TEXTURE_H
#define _TEXTURE_H

#include "../Utility/MathTypes.h"

namespace Orange
{

	// TODO: Possibly include other texture formats
	enum class TextureFormat : uint8_t
	{
		INVALID = 0,
		RGBA_32,
		R_8
	};

	enum class TextureDimensions : uint8_t
	{
		INVALID = 0,
		ONE,
		TWO,
		THREE
	};

	struct TextureData
	{
		TextureFormat format = TextureFormat::INVALID;
		TextureDimensions dimensions = TextureDimensions::INVALID;
		Vec3 size = Vec3(0.0f);
		void* data = nullptr;
	};

	// This texture class is a way of storing textures in a
	// renderer-agnostic way. A texture is really an array of data
	// of a specific format and size, which is exactly what this class follows
	class Texture
	{
	public:

		Texture() = default;
		Texture(const TextureData& data);
		Texture(const Texture& other);
		~Texture();

		const TextureData& GetTextureData() const;

		// This call that sets texture data will copy the data over to
		// an internal buffer. If copying is not intended and you just
		// want to hold a reference to an existing texture, see SetReferenceDataToTexture()
		void SetTextureData(const TextureData& data);

		// Sets texture data but does not copy to internal buffer (ie. does not call Create())
		void SetReferenceDataToTexture(const TextureData& data);

	private:

		void Create();
		void Destroy();

		// Returns the size of each unit (pixel) of data, depending on
		// the format, in BYTES
		uint32_t GetUnitSize();

		TextureData m_texData;

		bool m_isReference;

	};

}

#endif