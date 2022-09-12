#ifndef _FONTLOADER_H
#define _FONTLOADER_H

#include <unordered_map>
#include "MathTypes.h"
#include "../Core/Texture.h"

namespace Orange
{

	struct FontManager_CharacterData
	{
		Texture textureHandle;		// Glyph texture data
		Vec2 bearing;				// Offset from the baseline to the top-left of the glyph
		uint32_t advance;			// Offset to advance to next glyph
	};

	struct FontManager_FontStats
	{
		Vec2 maxCharSize = Vec2(0.0f);
		Vec2 minCharSize = Vec2(0.0f);
		float maxNegativeVerticalOffset = 0.0f; // Maximum vertical offset for characters such as 'g'
	};

	// The FontManager class is responsible for loading fonts and maintaining their data,
	// which can be queried by any other system at any time after Init()
	class FontManager
	{
	public:

		static void Initialize();

		static void Deinitialize();

		static const FontManager_CharacterData GetDataForChar(const char c);

		static const Vec2& GetLoadedFontSize();

		static const FontManager_FontStats& GetFontStats();

		static uint32_t GetVerticalOffsetForLineNumber(const uint32_t& lineNum);

		static uint32_t GetVerticalOffset();

		static void LoadFont(const char* fontName, const uint32_t fontSize);

	private:

		static void Reset();

		static void Convert8BitGrayscaleTo32BitWithPadding(uint8_t* sourceData, const Vec2& arraySize, uint32_t* targetData);

	private:

		static std::unordered_map<char, FontManager_CharacterData> m_charData;
		static FontManager_FontStats m_fontStats;

		static Vec2 m_loadedFontSize;

		static bool isInitialized;

	};

}

#endif