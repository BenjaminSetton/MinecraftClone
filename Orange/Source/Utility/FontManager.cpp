#include "../Misc/pch.h"

// Include FreeType header
#include <ft2build.h>
#include FT_FREETYPE_H

#include "FileSystem/FileSystem.h"
#include "FontManager.h"
#include "../Utility/HeapOverrides.h"
#include "MathTypes.h"
#include "Utility.h"

namespace Orange
{

	// Static variable declarations
	std::unordered_map<char, FontManager_CharacterData> FontManager::m_charData = std::unordered_map<char, FontManager_CharacterData>();
	FontManager_FontStats FontManager::m_fontStats = { 0, };
	Vec2 FontManager::m_loadedFontSize = Vec2(0.0);
	bool FontManager::isInitialized = false;

	void FontManager::Initialize()
	{
		OG_ASSERT_MSG(!isInitialized, "Why are we re-initializing the FontManager?");

		LoadFont("verdana.ttf", 12);

		isInitialized = true;
	}

	void FontManager::Deinitialize()
	{
		Reset();

		isInitialized = false;
	}

	const Orange::FontManager_CharacterData FontManager::GetDataForChar(const char c)
	{
		return m_charData.at(c);
	}

	const Orange::Vec2& FontManager::GetLoadedFontSize()
	{
		return m_loadedFontSize;
	}

	const FontManager_FontStats& FontManager::GetFontStats()
	{
		return m_fontStats;
	}

	uint32_t FontManager::GetVerticalOffsetForLineNumber(const uint32_t& lineNum)
	{
		return lineNum * GetVerticalOffset();
	}

	uint32_t FontManager::GetVerticalOffset()
	{
		return static_cast<uint32_t>(m_fontStats.maxCharSize.y + m_fontStats.maxNegativeVerticalOffset);
	}

	void FontManager::LoadFont(const char* fontName, const uint32_t fontSize)
	{
		FT_Library ftLibrary;
		if (FT_Init_FreeType(&ftLibrary))
		{
			OG_ASSERT_MSG(false, "Failed to initialize FreeType library!");
		}

		FT_Face face;
		std::string fontPath = FileSystem::GetFileNameRelativeToGeneratedDirectory(fontName);
		if (FT_New_Face(ftLibrary, FileSystem::GetFileNameRelativeToGeneratedDirectory(fontName).c_str(), 0, &face))
		{
			OG_ASSERT_MSG(false, "Failed to load font!");
		}

		// Define the pixel font size we want to extract. Setting the width parameter to 0 will
		// make the function calculate it based on the height, which is what we do here.
		FT_Set_Pixel_Sizes(face, 0, fontSize);

		// Set the pixel size of the font we're currently loading
		// TODO - Figure out how to retrieve the font width in pixels, setting to 0 for now
		m_loadedFontSize = { 0.0f, static_cast<float>(fontSize) };

		// Load in the first 128 characters
		Vec2 bearing = Vec2(0.0f);
		Vec2 size = Vec2(0.0f);
		uint32_t advance = 0;
		for (unsigned char c = 0; c < 128; c++)
		{
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				OG_ASSERT_MSG(false, "Failed to load char!");
				continue;
			}

			// Gather the data from the newly-loaded face
			bearing = { face->glyph->bitmap_left, face->glyph->bitmap_top };
			advance = static_cast<uint32_t>(face->glyph->advance.x) >> 6; // Get value in pixels!
			size = { static_cast<float>(face->glyph->bitmap.width), static_cast<float>(face->glyph->bitmap.rows) };

			// Create the new texture
			TextureSpecs texData;
			texData.format = TextureFormat::RGBA_32;
			texData.dimensions = TextureDimensions::TWO;
			texData.size = { size.x, size.y, 0.0f };


			// Get the accumulated font stats
			m_fontStats.maxCharSize = { max(m_fontStats.maxCharSize.x, size.x), max(m_fontStats.maxCharSize.y, size.y) };
			m_fontStats.minCharSize = { min(m_fontStats.minCharSize.x, size.x), min(m_fontStats.minCharSize.y, size.y) };
			m_fontStats.maxNegativeVerticalOffset = max(m_fontStats.maxNegativeVerticalOffset, size.y - bearing.y);

			// Convert from 8-bit gray scale to a regular 32-bit texture. This will allow the UI Renderer to be a lot simpler, since
			// we won't have to deal with a separate case for sampling an 8-bit texture in the shaders. We can just use a 32-bit texture for everything
			uint32_t arraySize = static_cast<uint32_t>(size.x * size.y);
			uint32_t* newBuffer = OG_NEW uint32_t[arraySize];
			Convert8BitGrayscaleTo32BitWithPadding(static_cast<uint8_t*>(face->glyph->bitmap.buffer), size, newBuffer);

			// Cache the character data
			FontManager_CharacterData character =
			{
				Texture(texData, newBuffer),
				bearing,
				advance
			};

			// We can now delete this temporary buffer, since the Texture object will have created it's own copy of the buffer
			delete[] newBuffer;

			// Finally, add the character data to the map
			m_charData[c] = character;
		}

		// Finally, release FT's resources
		FT_Done_Face(face);
		FT_Done_FreeType(ftLibrary);
	}

	void FontManager::Reset()
	{
		m_charData.clear();
	}


	void FontManager::Convert8BitGrayscaleTo32BitWithPadding(uint8_t* sourceData, const Vec2& arraySize, uint32_t* targetData)
	{
		// Convert to char* because it'll allow us to get each byte by simply indexing
		for (uint32_t height = 0; height < arraySize.y; height++)
		{
			for (uint32_t width = 0; width < arraySize.x; width++)
			{
				uint32_t index = static_cast<uint32_t>(arraySize.x) * height + width;
				uint8_t currentPaddedByte = sourceData[index];

				// Since we're rendering text, we want to set the color to white and the alpha
				// to whatever value the grayscale texture has. We can alter the color of the text
				// by using a separate float4 in the shaders
				uint32_t finalResult = 0x00FFFFFF | (currentPaddedByte << 24);
				targetData[index] = finalResult;
			}
		}
	}

}