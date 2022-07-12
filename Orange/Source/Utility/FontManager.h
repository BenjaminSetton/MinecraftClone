#ifndef _FONTLOADER_H
#define _FONTLOADER_H

#include <unordered_map>
#include "MathTypes.h"
#include "../Core/Texture.h"

namespace Orange
{

	struct FontManager_CharacterData
	{
		Texture* texture;			// Glyph texture data
		Vec2 bearing;				// Offset from the baseline to the top-left of the glyph
		uint32_t advance;			// Offset to advance to next glyph
	};

	// The FontManager class is responsible for loading fonts and maintaining their data,
	// which can be queried by any other system at any time after Init()
	class FontManager
	{
	public:

		static void Initialize(const std::string fontName = "verdana.ttf", const uint32_t fontSize = 75);

		static void Deinitialize();

		static const FontManager_CharacterData GetDataForChar(const char c);

	private:

		static void Reset();

	private:

		static std::unordered_map<char, FontManager_CharacterData> m_charData;

	};

}

#endif