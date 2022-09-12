#ifndef _TEXTURE_H
#define _TEXTURE_H

#include "../Utility/MathTypes.h"
#include "../Utility/Utility.h"

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

	struct TextureSpecs
	{
		TextureFormat format = TextureFormat::INVALID;
		TextureDimensions dimensions = TextureDimensions::INVALID;
		Vec3 size = Vec3(0.0f, 0.0f, 0.0f);
	};

	// This texture class is a way of storing textures in a
	// renderer-agnostic way. A texture is really an array of data
	// of a specific format and size, which is exactly what this class follows
	class Texture
	{
	public:

		Texture();
		Texture(const TextureSpecs& specs, const void* data);
		Texture(const Texture& other);
		~Texture() = default;

		const TextureSpecs& GetSpecs() const;
		Ref<void> GetData() const;
		const uint64_t& GetId() const;

		// Creates a one-pixel-sized texture of the specified color. This is useful for binding
		// to shaders as default textures
		void CreateSolidColorTexture(const Vec4& color);

		const bool IsValid() const;

		// Returns the size of each unit (pixel) of data, depending on
		// the format, in BYTES
		uint32_t GetUnitSize() const;

	private:

		void Create(const void* data);

		uint32_t ConvertVec4IntoRGBA32(const Vec4& color);

		void AllocateAndPopulateBuffer(const void* data, const uint64_t numBytes);

		TextureSpecs m_specs;

		// Data is automatically cleaned up when ref_count == 0. This means that the texture registry can
		// return copies of a texture object, and the user can be guaranteed that the object will exist
		// when they are using it, even if the "real" texture object is deleted from the registry
		Ref<void> m_data;

		// A UUID that describes this texture object. This is used to tie all the dependencies together.
		// For example, there exists another map somewhere in the code that contains a map from a tex ID
		// to a SRV that is passed onto the shader. The ID allows us to always retrieve the same texture
		uint64_t m_id = 0;

	};

}

#endif