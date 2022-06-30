#ifndef _TEXTURE_H
#define _TEXTURE_H

#include <cstdint>
#include <cstring>

enum class TextureFormat
{
	RGBA = 0,
	// TODO: Possibly include other texture formats
};

// This base texture class is a way of storing textures in a
// renderer-agnostic way. A texture is really an array of pixels
// of a specific format, which is exactly what this class follows
class Texture
{
public:

	Texture() = default;
	Texture(const Texture& other) = default;
	~Texture() = default;

	const TextureFormat GetFormat();
	void* GetData() const;

protected:

	virtual void OverridePlease() = 0;

	TextureFormat m_format;
	void* m_data;

};

#endif