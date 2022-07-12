#include "../Misc/pch.h"

// Include FreeType header
#include <ft2build.h>
#include FT_FREETYPE_H

#include "FileSystem/FileSystem.h"
#include "FontManager.h"
#include "MathTypes.h"
#include "Utility.h"

namespace Orange
{

	std::unordered_map<char, FontManager_CharacterData> FontManager::m_charData = std::unordered_map<char, FontManager_CharacterData>();

	void FontManager::Initialize(const std::string fontName, const uint32_t fontSize)
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

		// Load in the first 128 characters
		for (unsigned char c = 0; c < 128; c++)
		{
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				OG_ASSERT_MSG(false, "Failed to load char '%c'!", c);
				continue;
			}

			// Create the new texture
			TextureData texData;
			texData.format = TextureFormat::R_8;
			texData.dimensions = TextureDimensions::TWO;
			texData.data = face->glyph->bitmap.buffer;
			texData.size = { static_cast<float>(face->glyph->bitmap.width), static_cast<float>(face->glyph->bitmap.rows), 0.0f };
			Texture* charTexture = new Texture(texData);
			
			// Cache the character data
			FontManager_CharacterData character = 
			{
				charTexture,
				Vec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				static_cast<uint32_t>(face->glyph->advance.x)
			};

			m_charData[c] =  character;
		}

		// Finally, release FT's resources
		FT_Done_Face(face);
		FT_Done_FreeType(ftLibrary);
	}

	void FontManager::Deinitialize()
	{
		Reset();
	}

	const Orange::FontManager_CharacterData FontManager::GetDataForChar(const char c)
	{
		return m_charData.at(c);
	}

	void FontManager::Reset()
	{
		// Delete the dynamically-allocated textures
		for (auto iter : m_charData)
		{
			delete iter.second.texture;
		}

		m_charData.clear();
	}

}