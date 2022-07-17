
#include <stdarg.h>

#include "../../Utility/FontManager.h"
#include "../../Utility/MathTypes.h"
#include "UIBuffer.h"
#include "UIHelper.h"
#include "../../Utility/Utility.h"

static constexpr uint32_t MAX_TEXT_BUFFER_SIZE = 1024;

static Orange::UIContextDescription g_ContextDesc;

namespace Orange
{
	namespace UI
	{

		const UIContextDescription* GetContextDescriptionRO()
		{
			return &g_ContextDesc;
		}

		UIContextDescription* GetContextDescription()
		{
			return &g_ContextDesc;
		}

		void Text(const char* format, ...)
		{
			char tempBuffer[MAX_TEXT_BUFFER_SIZE];
			va_list args;
			va_start(args, format);
			uint32_t numCharsWritten = vsnprintf(tempBuffer, sizeof(tempBuffer), format, args);

			// Assert that we're not going over the max text limit
			// otherwise, vsnprintf will not populate tempBuffer
			if (numCharsWritten > MAX_TEXT_BUFFER_SIZE - 1)
			{
				OG_ASSERT_MSG(false, "Text is too large to store in temporary buffer! Consider cutting down on text or increasing the max buffer text size");
				return;
			}

			va_end(args);

			// Loop through all chars of the formatted string, create a quad and draw command for each char and send to UIBuffer
			Vec2 prevPos = Vec2(100, 250);
			for (uint32_t i = 0; i < numCharsWritten; i++)
			{
				char c = tempBuffer[i];
				FontManager_CharacterData charData = FontManager::GetDataForChar(c);

				// Create the 6 corresponding vertices for the quad and push to UIBuffer
				Vec2 size = { charData.texture->GetTextureData().size.x, charData.texture->GetTextureData().size.y };
				Vec2 newPos = { prevPos.x + charData.bearing.x, prevPos.y - (size.y - charData.bearing.y) };
				Vec3 colorModifier = g_ContextDesc.colorMultiplier;

				UIVertex newVertices[6] =
				{
					// POSITION            SIZE	 POS     COLOR
					{ QUAD_COORDINATES[0], size, newPos, colorModifier },
					{ QUAD_COORDINATES[1], size, newPos, colorModifier },
					{ QUAD_COORDINATES[2], size, newPos, colorModifier },
					{ QUAD_COORDINATES[3], size, newPos, colorModifier },
					{ QUAD_COORDINATES[4], size, newPos, colorModifier },
					{ QUAD_COORDINATES[5], size, newPos, colorModifier }
				};

				for (uint32_t j = 0; j < 6; j++)
				{
					UIBuffer::PushUIVertex(newVertices[j]);
				}

				// Create and push the draw command
				UIDrawCommand drawCommand;
				drawCommand.textureHandle = charData.texture;
				drawCommand.type = UIElementType::TEXT;
				drawCommand.drawContext.text.c = c;

				UIBuffer::PushDrawCommand(drawCommand);

				// Get the position of the next character. Bit-shift to give value in pixels
				prevPos.x += static_cast<float>(charData.advance >> 6);
			}
		}

		void Image(void* data, const Vec2 size)
		{
			// Create the 6 corresponding vertices for the quad and send to UIBuffer
			Vec2 position = Vec2(100, 250);
			Vec3 colorModifier = g_ContextDesc.colorMultiplier;

			UIVertex newVertices[6] =
			{
				// POSITION            SIZE	 POSITION  COLOR
				{ QUAD_COORDINATES[0], size, position, colorModifier },
				{ QUAD_COORDINATES[1], size, position, colorModifier },
				{ QUAD_COORDINATES[2], size, position, colorModifier },
				{ QUAD_COORDINATES[3], size, position, colorModifier },
				{ QUAD_COORDINATES[4], size, position, colorModifier },
				{ QUAD_COORDINATES[5], size, position, colorModifier }
			};

			for (uint32_t i = 0; i < 6; i++)
			{
				UIBuffer::PushUIVertex(newVertices[i]);
			}

			TextureData texData;
			texData.data = data;
			texData.dimensions = TextureDimensions::TWO;
			texData.format = TextureFormat::RGBA_32;
			texData.size = { size.x, size.y, 0.0f };

			UIDrawCommand drawCommand;
			drawCommand.textureHandle->SetTextureData(texData);
			drawCommand.type = UIElementType::IMAGE;

			UIBuffer::PushDrawCommand(drawCommand);
		}
	}
}