#ifndef _TEXTURE_H
#define _TEXTURE_H

#include <cstdint>
#include <cstring>

enum class TextureFormat
{
	RGBA = 0,
	// TODO: Possibly include other texture formats
};

// The high-level implementation of a texture
class Texture
{
public:

	Texture() = default;
	Texture(const Texture& other) 
	{
		// TODO: CHECK ME!!
		const int32_t width = other.m_width;
		const int32_t height = other.m_height;
		m_width = width;
		m_height = height;
		m_format = other.m_format;
		int32_t* data = new int32_t[width][height];
		// Assumes a texture format size of 32bpp
		memcpy(m_data, other.m_data, sizeof(int32_t) * width * height);
		m_data = static_cast<void*>(data);
	};
	~Texture() = default;

	virtual void Create(const int32_t width, const int32_t height, const TextureFormat format, void* data) = 0;
	virtual void Destroy() = 0;

	const int32_t GetWidth() const { return m_width; }
	const int32_t GetHeight() const { return m_height; }
	const TextureFormat GetFormat() const { return m_format; }

private:

	int32_t m_width;
	int32_t m_height;
	TextureFormat m_format;
	void* m_data;
};

#endif