
#include <functional>
#include <stdarg.h>
#include <sstream>

#include "../Application.h"
#include "../../Utility/FontManager.h"
#include "../../Utility/HeapOverrides.h"
#include "../../Utility/Input.h"
#include "../../Utility/Math.h"
#include "../../Utility/MathTypes.h"
#include "../../Core/TextureRegistry.h"
#include "UIHelper.h"
#include "../../Utility/Utility.h"

static constexpr uint32_t MAX_TEXT_BUFFER_SIZE = 1024;

static Orange::UIContext gContext;

static Orange::Texture gPlaceholderTexture;

namespace Orange
{
	namespace UI
	{

		////////////////////////////////////////////////////////////////////////////////
		//
		//
		//	HELPER FUNCTIONS
		//
		//
		////////////////////////////////////////////////////////////////////////////////

		uint32_t GetCharArrayLength(const char* arr)
		{
			uint32_t len = 0;
			while (arr[len] != '\0')
			{
#ifdef OG_DEBUG
				if (len > 10000) OG_ASSERT_MSG(false, "Passed in a string that's not null-terminated?");
#endif
				len++;
			}
			return len;
		}

		template<typename T>
		std::string ToString(T arg)
		{
			std::stringstream ss;
			ss << arg;
			return ss.str();
		}

		// TODO - Rework so this operation is not O(N)
		uint32_t GetContainerDepth(const UIHash& containerHash)
		{
			for (uint32_t depth = 0; depth < gContext.ZMap.size(); depth++)
			{
				UIHash hash = gContext.ZMap[depth];
				if (hash == containerHash) return depth;
			}

			// Hash is not in the depth map
			return INVALID_UIHASH;
		}

		bool IsContainer(const UIHash& hash)
		{
			return (gContext.containerList.find(hash) != gContext.containerList.end());
		}

		// Returns the hash of the highest container we're hovering over,
		// and also caches the type of widget we're interacting with in
		// the global context
		UIHash FindHoveredContainerWidget()
		{
			Vec2 mousePos = Input::GetMousePositionRelativeToBottomLeft();
			UIHash highestContainer = INVALID_UIHASH;
			uint32_t maxDepth = 0;
			ContainerWidget hoveredWidget = ContainerWidget::INVALID;
			bool isHoveringAnyWidget = false;

			for (auto containerIter : gContext.containerList)
			{
				UIContainer* container = containerIter.second;

				// Allow the user to move the container by clicking on the titlebar (no titlebar = no moving around)
				if (container->titleBarRect.IsPointInRect(mousePos))
				{
					hoveredWidget = ContainerWidget::TITLEBAR;
				}
				// Allow the user to resize by clicking on the resize nook
				else if (container->resizeNookRect.IsPointInRect(mousePos))
				{
					hoveredWidget = ContainerWidget::RESIZE_NOOK;
				}
				else if (container->containerRect.IsPointInRect(mousePos))
				{
					hoveredWidget = ContainerWidget::CONTAINER_BODY;
				}
				else
				{
					hoveredWidget = ContainerWidget::INVALID;
				}

				if (hoveredWidget != ContainerWidget::INVALID)
				{
					uint32_t containerDepth = GetContainerDepth(container->hash);
					if (containerDepth >= maxDepth)
					{
						maxDepth = containerDepth;
						highestContainer = container->hash;
						gContext.hoveredContainerWidget = hoveredWidget;
					}

					isHoveringAnyWidget = true;
				}
			}

			// If we're not hovering any widget, set the hovered widget to invalid
			if (!isHoveringAnyWidget)
			{
				gContext.hoveredContainerWidget = ContainerWidget::INVALID;
			}

			return highestContainer;
		}

		UIHash GetHashForString(const char* str)
		{
			std::hash<const char*> hasher;
			return hasher(str);
		}

		UIContainer* GetContainer()
		{
			OG_ASSERT(!gContext.containerStack.empty());
			return gContext.containerStack.top();
		}

		// The "position" parameter is the bottom-left of a quad, with a positive size
		// corresponding to growth towards the top-right
		void EmplaceQuadVertices(const Vec2 position, const Vec2 size, const Vec4 color)
		{
			UIVertex newVertices[6] =
			{
				// POSITION            SIZE	 POS     COLOR
				{ QUAD_COORDINATES[0], size, position, color },
				{ QUAD_COORDINATES[1], size, position, color },
				{ QUAD_COORDINATES[2], size, position, color },
				{ QUAD_COORDINATES[3], size, position, color },
				{ QUAD_COORDINATES[4], size, position, color },
				{ QUAD_COORDINATES[5], size, position, color }
			};

			for (uint32_t j = 0; j < 6; j++)
			{
				gContext.vertexList.emplace(newVertices[j]);
			}
		}

		void EmplaceDrawCommand(const UIDrawCommand& drawCommand)
		{
			gContext.drawCommandList.emplace(drawCommand);
		}

		Vec2 DrawQuad_Internal(const UIRect& quadRect, const Vec4& color, const Texture& tex, const UIElementType& elementType)
		{
			// Emplace the quad and draw command
			EmplaceQuadVertices(quadRect.GetMin(), quadRect.GetSize(), color);

			UIDrawCommand drawCommand;
			drawCommand.textureHandle = tex;
			drawCommand.type = elementType;
			if (elementType == UIElementType::CONTAINER)
			{
				drawCommand.scissorRect = &GetContainer()->containerRect;
			}
			EmplaceDrawCommand(drawCommand);

			return quadRect.GetSize();
		}

		// Calculates position of text quads using parameters and enqueues the vertices and draw commands to the draw queues
		Vec2 DrawText_Internal(const char* text, const Vec2 startPosition, const bool wrapText, const Vec2 sizeLimit, const Vec2 padding, const HOR_ALIGNMENT& horAlign = HOR_ALIGNMENT::LEFT, const VER_ALIGNMENT& verAlign = VER_ALIGNMENT::TOP)
		{
			if (sizeLimit.x <= padding.x)
			{
				OG_LOG_WARNING("Skipped text draw. Can't have horizontal padding greater than or equal to text's width limit");
				return Vec2(0);
			}
			//OG_ASSERT_MSG(padding.x < sizeLimit.x, "Can't have horizontal padding greater than or equal to text's width limit");

			bool hasVerticalSizeLimit = sizeLimit.y >= 0;
			if(hasVerticalSizeLimit)
			{
				if (sizeLimit.y <= padding.y)
				{
					OG_LOG_WARNING("Skipped text draw. Can't have vertical padding greater than or equal to text's height limit");
					return Vec2(0);
				}
				//OG_ASSERT_MSG(padding.y < sizeLimit.y, "Can't have vertical padding greater than or equal to text's height limit");
			}

			// Get the length of the container name
			uint32_t numChars = GetCharArrayLength(text);

			// Loop over all char data for entire text to determine specs for the entire text, such as
			// how much space it will use up (used for alignment). Char data is then cached to save computation
			std::vector<FontManager_CharacterData> charDataForText;
			charDataForText.reserve(numChars);
			Vec2 spaceUsed = Vec2(0);
			for (uint32_t i = 0; i < numChars; i++)
			{
				charDataForText.push_back(FontManager::GetDataForChar(text[i]));
				FontManager_CharacterData& charData = charDataForText[i];

				// Calculate how much space the text takes up horizontally, we
				// need special cases for the beginning and end
				if (i == 0)
				{
					spaceUsed.x += (charData.advance - charData.bearing.x);
				}
				else if(i == numChars - 1)
				{
					spaceUsed.x += (charData.bearing.x + charData.textureHandle.GetSpecs().size.x);
				}
				else
				{
					spaceUsed.x += (charData.advance);
				}

				// Consider how much vertical space it takes up
				spaceUsed.y = max(spaceUsed.y, charData.textureHandle.GetSpecs().size.y);
			}

			Vec2 prevPos = Vec2(0);

			// Dictate where we will start writing our chars to considering horizontal alignment
			switch (horAlign)
			{
			case HOR_ALIGNMENT::LEFT:
			{
				prevPos.x = padding.x;
				break;
			}
			case HOR_ALIGNMENT::CENTER:
			{
				prevPos.x = std::trunc((sizeLimit.x - spaceUsed.x) / 2.0f);
				break;
			}
			case HOR_ALIGNMENT::RIGHT:
			{
				prevPos.x = sizeLimit.x - spaceUsed.x - padding.x;
				break;
			}
			default:
			{
				OG_ERROR("Unknown alignment");
				break;
			}
			}

			// Dictate where we will start writing our chars to considering vertical alignment
			if (hasVerticalSizeLimit)
			{
				switch (verAlign)
				{
				case VER_ALIGNMENT::TOP:
				{
					prevPos.y = -padding.y;
					break;
				}
				case VER_ALIGNMENT::CENTER:
				{
					// TODO - Figure out how to deal with jittering when typing
					prevPos.y = -std::trunc((sizeLimit.y - spaceUsed.y) / 2.0f);
					break;
				}
				case VER_ALIGNMENT::BOTTOM:
				{
					prevPos.y = -(sizeLimit.y - spaceUsed.y - padding.y);
					break;
				}
				default:
				{
					OG_ERROR("Unknown alignment");
					break;
				}
				}
			}
			else
			{
				// Always include padding, so we're just defaulting to top alignment
				prevPos.y = -padding.y;
			}

			uint32_t currentLine = 0;
			uint32_t widthTaken = 0;
			for (uint32_t i = 0; i < numChars; i++)
			{
				char c = text[i];
				FontManager_CharacterData charData = charDataForText[i];

				Vec2 size = { charData.textureHandle.GetSpecs().size.x, charData.textureHandle.GetSpecs().size.y };
				// Bit-shift to give value in pixels
				float advanceToNextGlyph = static_cast<float>(charData.advance);
				Vec2 startQuadPosition(0);

				float negativeVerticalOffset = size.y - charData.bearing.y;
				if (wrapText)
				{
					// We don't fit in the current line, so go down by one row
					if (prevPos.x + charData.bearing.x + size.x >= sizeLimit.x - padding.x)
					{
						currentLine++;
						startQuadPosition = { padding.x, prevPos.y - negativeVerticalOffset - FontManager::GetVerticalOffsetForLineNumber(currentLine) };
						prevPos.x = startQuadPosition.x;
					}
					// Char fits within the current line
					else
					{
						startQuadPosition = { prevPos.x + charData.bearing.x, prevPos.y - negativeVerticalOffset - FontManager::GetVerticalOffsetForLineNumber(currentLine) };
					}
				}
				else
				{
					startQuadPosition = { prevPos.x + charData.bearing.x, prevPos.y - negativeVerticalOffset - FontManager::GetVerticalOffsetForLineNumber(currentLine) };
				}

				widthTaken = min(static_cast<uint32_t>(prevPos.x + charData.bearing.x + size.x), static_cast<uint32_t>(sizeLimit.x));

				// Text starts at top-left of the container
				Vec2 vertexPos = Vec2
				(
					startPosition.x + startQuadPosition.x,
					startPosition.y + startQuadPosition.y - spaceUsed.y/*FontManager::GetFontStats().maxCharSize.y*/
				);

				// Advance to the next glyph
				prevPos.x += advanceToNextGlyph;

				// NOTE: This should be controlled by some sort of style var that gets pushed onto a stack and popped
				// Exactly like how ImGui handles this type of thing. Hard-coded to white for now
				Vec4 colorModifier = { 1.0f, 1.0f, 1.0f, 1.0f };

				EmplaceQuadVertices(vertexPos, size, colorModifier);

				// Create and push the draw command
				UIDrawCommand drawCommand; DEBUG_DRAW_COMMAND_FILE_AND_LINE(drawCommand)
				drawCommand.textureHandle = charData.textureHandle;
				drawCommand.drawContext.text.c = c;
				drawCommand.type = UIElementType::TEXT;

				gContext.drawCommandList.emplace(drawCommand);

			}

			// Return the amount of space used. The container uses this to calculate where the next components's position will start
			return Vec2(widthTaken, FontManager::GetVerticalOffsetForLineNumber(currentLine + 1));
		}

		const Vec2 DrawTitleBar_Internal(const UIRect& titleBarRect, const Texture& tex)
		{
			// Draw title-bar quad
			DrawQuad_Internal(titleBarRect, titleBarColor, tex, UIElementType::IMAGE);

			// Return the space used by the title-bar
			return titleBarRect.GetSize();
		}

		const Vec2 DrawCheckbox_Internal(const Vec2& startPosition, const Vec2& checkBoxSize, const Texture& tex, const bool* isTicked)
		{
			// Emplace the quad and draw command
			Vec2 size = checkBoxSize;
			Vec4 color = (*isTicked) ? checkBoxCheckedColor : checkBoxUncheckedColor;
			Vec2 position = startPosition;
			EmplaceQuadVertices(position, size, color);

			UIDrawCommand drawCommand; DEBUG_DRAW_COMMAND_FILE_AND_LINE(drawCommand)
			drawCommand.textureHandle = tex;
			drawCommand.type = UIElementType::CHECKBOX;
			EmplaceDrawCommand(drawCommand);

			// Return the space used by the checkbox
			return checkBoxSize;
		}

		const Vec2 DrawSlider_Internal(const UIRect& sliderBarRect, const UIRect& sliderHandleRect, const Texture& tex, const bool& isMouseHovering)
		{
			// Draw the slider bar quad
			DrawQuad_Internal(sliderBarRect, sliderBarColor, tex, UIElementType::IMAGE);

			// Draw the slider handle quad
			DrawQuad_Internal(sliderHandleRect, isMouseHovering ? sliderHandleSelectedColor : sliderHandleUnselectedColor, tex, UIElementType::IMAGE);

			return sliderBarRect.GetSize();
		}

		void DrawScrollBar_Internal()
		{

		}

		////////////////////////////////////////////////////////////////////////////////
		//
		//
		//	API FUNCTIONS
		//
		//
		////////////////////////////////////////////////////////////////////////////////

		const UIContext* GetContextRO()
		{
			return &gContext;
		}

		UIContext* GetContext()
		{
			return &gContext;
		}

		void Text(const char* format, ...)
		{
			// Assert that there's a container in the stack. Otherwise, this means that we are attempting to calls UI::XXXX() without
			// a matching Begin/End around it
			if (gContext.containerStack.size() == 0)
			{
				OG_ASSERT_MSG(false, "Attempting to call UI::Text() without calling UI::Begin() first!");
				return;
			}

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

			// Get the container we're appending to
			UIContainer* container = GetContainer();

			// Draw the actual text
			Vec2 sizeLimit = Vec2(static_cast<int32_t>(container->GetRemainingSize().x), -1);
			Vec2 spaceUsed = DrawText_Internal(tempBuffer, container->GetRemainingSpace().GetCorner(RECT_CORNER::TOP_LEFT), container->IsDataFlagSet(UIContainer_WrapText), sizeLimit, container->padding);
			
			// Invalidate the space we just used for the text
			container->InvalidateSpace(Vec2(0.0f, spaceUsed.y + container->padding.y));
		}

		void Image(const Texture& tex)
		{
			Vec2 imageSize = Vec2(tex.GetSpecs().size.x, tex.GetSpecs().size.y);
			Vec2 imageExtent = imageSize / 2.0f;
			UIContainer* container = GetContainer();

			UIRect imageRect;
			imageRect.center = container->GetRemainingSpace().GetCorner(RECT_CORNER::TOP_LEFT) + Vec2(container->padding.x, -container->padding.y) + Vec2(imageExtent.x, -imageExtent.y);
			imageRect.extent = imageExtent;
			Vec2 spaceUsed = DrawQuad_Internal(imageRect, Vec4(1.0f, 1.0f, 1.0f, 1.0f), tex, UIElementType::IMAGE);

			container->InvalidateSpace(Vec2(0.0f, spaceUsed.y));
		}

		void Checkbox(bool* pBool, const char* format, ...)
		{
			if (pBool == nullptr) return;

			// Assert that there's a container in the stack. Otherwise, this means that we are attempting to calls UI::XXXX() without
			// a matching Begin/End around it
			if (gContext.containerStack.size() == 0)
			{
				OG_ASSERT_MSG(false, "Attempting to call UI::Text() without calling UI::Begin() first!");
				return;
			}

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

			UIContainer* container = GetContainer();

			// TODO - Include other variables in the hash calculation (bool state, widget type, etc)
			UIHash id = GetHashForString(tempBuffer);

			Vec2 checkBoxExtent = Vec2(10);
			Vec2 checkBoxStartingPos = Vec2(container->GetRemainingSpace().GetCorner(RECT_CORNER::TOP_LEFT).x + container->padding.x + checkBoxExtent.x, container->GetRemainingSpace().GetCorner(RECT_CORNER::TOP_LEFT).y - (FontManager::GetVerticalOffsetForLineNumber(1) / 2.0f) - container->padding.y);
			
			// Build a rect to check mouse collision with the checkbox
			UIRect checkBox = UIRect(checkBoxStartingPos, checkBoxExtent);

			Vec2 mousePos = Input::GetMousePositionRelativeToBottomLeft();
			if (checkBox.IsPointInRect(mousePos))
			{
				if (Input::IsMouseClicked(MouseCode::LBUTTON))
				{
					(*pBool) = !(*pBool);

					gContext.activeID = id;
				}

				gContext.hoveredID = id;
			}

			Vec2 spaceUsed = DrawCheckbox_Internal(checkBox.GetMin(), checkBoxExtent * 2.0f, gPlaceholderTexture, pBool);

			// Draw the text after the checkbox
			Vec2 textStartingPos = Vec2(container->GetRemainingSpace().GetCorner(RECT_CORNER::TOP_LEFT).x + checkBox.GetSize().x + container->padding.x, container->GetRemainingSpace().GetCorner(RECT_CORNER::TOP_LEFT).y - container->padding.y);
			Vec2 sizeLimit = Vec2(static_cast<uint32_t>(container->GetRemainingSize().x), static_cast<uint32_t>(checkBox.extent.y));
			DrawText_Internal(tempBuffer, textStartingPos, container->IsDataFlagSet(UIContainer_WrapText), sizeLimit, Vec2(container->padding.x, 0.0f), HOR_ALIGNMENT::LEFT, VER_ALIGNMENT::CENTER);

			// Invalidate space
			container->InvalidateSpace(Vec2(0.0f, spaceUsed.y + container->padding.y));

		}

		void Slider(float* value, const float min, const float max, const char* format, ...)
		{
			// Value between 0 and 1
			float tween = ((*value) - min) / (max - min);
			Math::Clamp(tween, 0.0f, 1.0f);

			UIContainer* container = GetContainer();
			Vec2 spaceUsed = Vec2(0);

			// Draw the text after the slider (the parameter)
			char tempBuffer[MAX_TEXT_BUFFER_SIZE];
			va_list args;
			va_start(args, format);
			uint32_t numCharsWritten = vsnprintf(tempBuffer, sizeof(tempBuffer), format, args);
			if (numCharsWritten > MAX_TEXT_BUFFER_SIZE - 1)
			{
				OG_ASSERT_MSG(false, "Text is too large to store in temporary buffer! Consider cutting down on text or increasing the max buffer text size");
				return;
			}
			va_end(args);

			// TODO - Include other variables in the hash calculation (float value, min, max, widget type, etc)
			UIHash id = GetHashForString(tempBuffer);

			UIRect barRect;
			barRect.center = Vec2(container->remainingSpace.GetCorner(RECT_CORNER::TOP_LEFT).x + container->padding.x + (gContext.sliderBarSizeInPixels.x / 2.0f), container->remainingSpace.GetCorner(RECT_CORNER::TOP_LEFT).y - container->padding.y - (gContext.sliderBarSizeInPixels.y / 2.0f));
			barRect.extent = gContext.sliderBarSizeInPixels / 2.0f;
			
			Vec2 handleSize = gContext.sliderHandleSizeInPixels;
			float realAvailableSliderDistance = barRect.GetSize().x - handleSize.x;
			float halfHandleWidth = (handleSize.x / 2.0f);
			UIRect handleRect;
			handleRect.center = Vec2(barRect.GetMin().x + (realAvailableSliderDistance * tween) + halfHandleWidth, barRect.center.y);
			handleRect.extent = Vec2(handleSize.x / 2.0f, handleSize.y / 2.0f);

			// Consider the mouse dragging the slider around, and update the rect to reflect it
			Vec2 mousePos = Input::GetMousePositionRelativeToBottomLeft();
			if (handleRect.IsPointInRect(mousePos))
			{
				if (Input::IsMouseDown(MouseCode::LBUTTON))
				{
					// Set this widget as active
					gContext.activeID = id;
				}

				// Set this widget as hovered
				gContext.hoveredID = id;
			}
			else
			{
				if (gContext.hoveredID == id) gContext.hoveredID = INVALID_UIHASH;
			}

			// Logic to drag slider around
			if (gContext.activeID == id)
			{
				// Set this widget as hovered because we're actively moving the handle
				gContext.hoveredID = id;

				float handlePosX = Input::GetMousePosition().x;
				float maxHandlePos = barRect.GetMax().x - handleRect.extent.x;
				float minHandlePos = barRect.GetMin().x + handleRect.extent.x;

				Math::Clamp(handlePosX, minHandlePos, maxHandlePos);

				float handlePosNorm = 1.0f - ((maxHandlePos - handlePosX) / (maxHandlePos - minHandlePos));
				OG_ASSERT(handlePosNorm >= 0.0 && handlePosNorm <= 1.0);

				// Set the new float value
				(*value) = ((max - min) * handlePosNorm) + min;

				// Set the center of the handle
				handleRect.center.x = handlePosX;
			}

			spaceUsed = DrawSlider_Internal(barRect, handleRect, gPlaceholderTexture, gContext.hoveredID == id);

			Vec2 textStartPosition = container->remainingSpace.GetCorner(RECT_CORNER::TOP_LEFT) + Vec2(barRect.GetSize().x + container->padding.x, -container->padding.y);
			Vec2 sizeLimit = Vec2(static_cast<uint32_t>(container->containerRect.GetMax().x - textStartPosition.x), static_cast<uint32_t>(barRect.GetSize().y));
			DrawText_Internal(tempBuffer, textStartPosition, false, sizeLimit, container->padding.x, HOR_ALIGNMENT::LEFT, VER_ALIGNMENT::CENTER);

			// Format the interpolated value as a string
			char formattedValue[20];
			int charsWritten = sprintf_s(formattedValue, gContext.sliderDecimalPrecisionFormat, *value);
			if (charsWritten < 0 || charsWritten > 19)
			{
				OG_ERROR("Failed to format the slider's value as string");
			}

			// Draw the current value of the variable
			sizeLimit = Vec2(static_cast<uint32_t>(barRect.GetSize().x), static_cast<uint32_t>(barRect.GetSize().y));
			DrawText_Internal(formattedValue, barRect.GetCorner(RECT_CORNER::TOP_LEFT), false, sizeLimit, container->padding.x, HOR_ALIGNMENT::CENTER, VER_ALIGNMENT::CENTER);

			// Invalidate the space for the slider widget
			container->InvalidateSpace(Vec2(0.0f, spaceUsed.y + container->padding.y));
		}

		void Begin(const char* containerName)
		{
			// Check that we have matching pairs of Begin/End calls
			// Currently we do not support nested Begin/End calls
			if (gContext.containerStack.size() != 0)
			{
				OG_ASSERT_MSG(false, "Calling UI::Begin() without a matching UI::End()!");
				return;
			}

			UIContainer* container = nullptr;
			UIHash containerHash = GetHashForString(containerName);

			if (!gContext.containerList.contains(containerHash))
			{
				// If the container is NOT already inside the containerList, create a new one.
				gContext.containerList[containerHash] = OG_NEW UIContainer();
			}
			container = gContext.containerList.at(containerHash);

			// Push it onto the stack
			gContext.containerStack.push(container);

			container->hash = containerHash;
			gContext.parentID = containerHash;

			// Calculate the depth of this container
			bool containerFoundInDepthMap = false;
			for (uint32_t i = 0; i < gContext.ZMap.size(); i++)
			{
				UIHash hash = gContext.ZMap[i];
				if (hash == containerHash)
				{
					// Swap this hash to the last position of the map
					gContext.ZMap.erase(gContext.ZMap.begin() + i);
					gContext.ZMap.push_back(hash);

					containerFoundInDepthMap = true;
					break;
				}
			}

			// If we don't find this container in the depth map, we just have to add it
			// to the end of the depth map
			if (!containerFoundInDepthMap)
			{
				gContext.ZMap.push_back(containerHash);
			}

			// Draw the container
			DrawQuad_Internal(container->containerRect, container->color, gPlaceholderTexture, UIElementType::CONTAINER);

			// Append the title-bar, if necessary
			if (container->IsDataFlagSet(UIContainer_ShowTitleBar))
			{
				Vec2 titleBarExtent = Vec2(container->containerRect.extent.x, static_cast<float>(container->titleBarHeight));
				UIRect titlebarRect;
				titlebarRect.center = Vec2(container->containerRect.GetCorner(RECT_CORNER::TOP_LEFT).x + titleBarExtent.x, container->containerRect.GetCorner(RECT_CORNER::TOP_LEFT).y - titleBarExtent.y);
				titlebarRect.extent = titleBarExtent;
				Vec2 spaceUsed = DrawTitleBar_Internal(titlebarRect, gPlaceholderTexture);

				container->titleBarRect = titlebarRect;

				// Draw the name of the container
				Vec2 sizeLimit = Vec2(static_cast<uint32_t>(titleBarExtent.x * 2.0f), static_cast<uint32_t>(titleBarExtent.y * 2.0f));
				DrawText_Internal(containerName, container->containerRect.GetCorner(RECT_CORNER::TOP_LEFT), false, sizeLimit, container->padding, HOR_ALIGNMENT::CENTER, VER_ALIGNMENT::CENTER);

				// Invalidate the space from the title-bar. Note that it
				// doesn't take up any extra horizontal space unlike the scroll-bar
				container->InvalidateSpace(Vec2(0.0f, spaceUsed.y));
			}
			else
			{
				container->titleBarRect = UIRect(Vec2(0), Vec2(0));
			}

			// Re-center the resize nook
			container->resizeNookRect.center = container->containerRect.GetCorner(RECT_CORNER::BOTTOM_RIGHT) - Vec2(container->resizeNookRect.extent.x, -container->resizeNookRect.extent.y);

		}

		void End()
		{
			// Check that we have matching pairs of Begin/End calls
			// Currently we do not support nested Begin/End calls
			if (gContext.containerStack.size() == 0)
			{
				OG_ASSERT_MSG(false, "Calling UI::End() without a matching UI::Begin()!");
				return;
			}

			// Draw the items that show on top of everything that could potentially be pushed
			// between the Begin and End() calls
			UIContainer* container = GetContainer();

			DrawQuad_Internal(container->resizeNookRect, titleBarColor, gPlaceholderTexture, UIElementType::IMAGE); // Resize nook
			//DrawQuad_Internal(container->scrollbarRect, Vec4(1), gPlaceholderTexture, UIElementType::IMAGE); // Scroll bar

			// Pop the container off the stack
			gContext.containerStack.pop();
		}

		void BeginFrame()
		{
			
		}

		void EndFrame()
		{
			OG_ASSERT_MSG(gContext.containerStack.size() == 0, "Container stack should be empty during EndFrame() call");
			OG_ASSERT_MSG(gContext.drawCommandList.size() == 0, "Draw command queue should be empty during EndFrame() call");
			OG_ASSERT_MSG(gContext.vertexList.size() == 0, "Vertex queue should be empty during EndFrame() call");

			// Clear all the used space of the containers
			for (auto iter : gContext.containerList)
			{
				UIContainer* currContainer = iter.second;
				currContainer->ResetDynamicData();
				currContainer->color = defaultBackgroundColor;
			}
		}

		void Update(const float& dt)
		{
			UNUSED(dt);

			// Reset some data if the LMB is up
			if (!Input::IsMouseDown(MouseCode::LBUTTON))
			{
				gContext.activeID = INVALID_UIHASH;
				gContext.activeIDTimer = 0;
				gContext.distToCenterOfActiveContainer = Vec2(0);
				gContext.currentActions = WidgetActions::NONE;
			}

			Vec2 mousePos = Input::GetMousePositionRelativeToBottomLeft();

			// We're dragging the container around, so update it's position.
			// We also don't care whether we're hovering over another container
			// or w/e else might be happening, so this check should go first
			if (IsContainer(gContext.activeID) && (gContext.activeID != INVALID_UIHASH))
			{
				UIContainer* activeContainer = gContext.containerList[gContext.activeID];

				switch (gContext.currentActions)
				{
				case WidgetActions::MOVING:
				{
					activeContainer->containerRect.center = Vec2(mousePos + gContext.distToCenterOfActiveContainer);

					if (activeContainer->IsDataFlagSet(UIContainer_KeepInWindowBounds))
					{
						Vec2 windowDimensions = Application::Handle->GetMainWindow()->GetSize();
						Vec2 rectExtent = activeContainer->containerRect.extent;

						activeContainer->containerRect.center.x = max(0.0f + rectExtent.x, activeContainer->containerRect.center.x);
						activeContainer->containerRect.center.x = min(windowDimensions.x - rectExtent.x, activeContainer->containerRect.center.x);
						activeContainer->containerRect.center.y = max(0.0f + rectExtent.y, activeContainer->containerRect.center.y);
						activeContainer->containerRect.center.y = min(windowDimensions.y - rectExtent.y, activeContainer->containerRect.center.y);
					}

					break;
				}
				case WidgetActions::RESIZING:
				{
					// Calculate the new width and height, and don't allow the container to become smaller than the minimum size
					float newWidth = max((mousePos.x - gContext.topLeftPositionWhenResizing.x) / 2.0f, gContext.minContainerSize.x);
					float newHeight = max((gContext.topLeftPositionWhenResizing.y - mousePos.y) / 2.0f, gContext.minContainerSize.y);
					
					// We only want the extent to be an integer -- truncate decimals
					activeContainer->containerRect.extent = Vec2(trunc(newWidth), trunc(newHeight));
					
					// Re-adjust the center
					Vec2 newTopLeft = activeContainer->containerRect.GetCorner(RECT_CORNER::TOP_LEFT);
					Vec2 oldTopLeft = gContext.topLeftPositionWhenResizing;
					activeContainer->containerRect.center += Vec2(oldTopLeft.x - newTopLeft.x, -(newTopLeft.y - oldTopLeft.y));
					break;
				}
				case WidgetActions::SCROLLING:
				{
					break;
				}
				case WidgetActions::NONE:
				{
					break;
				}
				default:
				{
					OG_ERROR("Please add the new widget action to this switch case");
				}
				}

				// Keep the color as "selected", even though the cursor could briefly move out of the container
				activeContainer->color = selectedBackgroundColor;
			}

			// Find the active and hovered container
			UIHash highestContainerHash = FindHoveredContainerWidget();

			// We're not hovering over any container, so set hoveredID to 0 and bail
			gContext.hoveredID = INVALID_UIHASH;
			if (highestContainerHash == INVALID_UIHASH) return;

			UIContainer* hotContainer = gContext.containerList[highestContainerHash];
			bool isMouseDown = Input::IsMouseDown(MouseCode::LBUTTON);

			gContext.hoveredID = highestContainerHash;
			if (hotContainer->IsDataFlagSet(UIContainer_CanHighlight))
			{
				hotContainer->color = selectedBackgroundColor;
			}

			if (isMouseDown && (gContext.activeID == INVALID_UIHASH))
			{
				switch (gContext.hoveredContainerWidget)
				{
				case ContainerWidget::TITLEBAR:
				{
					if (hotContainer->IsDataFlagSet(UIContainer_CanCursorLock))
					{
						if (gContext.currentActions == WidgetActions::MOVING) break;

						gContext.currentActions = WidgetActions::MOVING;

						// Set the distance to the center of the container in the context
						gContext.distToCenterOfActiveContainer = hotContainer->containerRect.center - mousePos;
					}
					break;
				}
				case ContainerWidget::RESIZE_NOOK:
				{
					if (hotContainer->IsDataFlagSet(UIContainer_CanResize))
					{
						if (gContext.currentActions == WidgetActions::RESIZING) break;

						gContext.currentActions = WidgetActions::RESIZING;

						// Store the current top-left coordinate of the container, so
						// we can resize it without moving it around
						gContext.topLeftPositionWhenResizing = hotContainer->containerRect.GetCorner(RECT_CORNER::TOP_LEFT);
					}

					break;
				}
				case ContainerWidget::SCROLLBAR:
				{
					gContext.currentActions = WidgetActions::SCROLLING;
					break;
				}
				case ContainerWidget::INVALID:
				{
					OG_ERROR("We can't click on an invalid widget");
				}
				}
				gContext.activeID = highestContainerHash;
			}

			// Reset the depth map
			memset(&gContext.ZMap[0], INVALID_UIHASH, gContext.ZMap.size() * sizeof(UIHash));
		}

		void Initialize()
		{
			// Do INI parsing and other necessary stuff
			
			// Create a placeholder texture
			//uint32_t placeholderWidth = 256;
			//uint32_t placeholderHeight = 256;
			//auto gen = [&](uint32_t x, uint32_t y)
			//{
			//	uint32_t currColor = 0xFF000000;
			//	uint32_t r = (sin(50.0f * x) * 0.5f + 0.5f) * 0xFF;
			//	uint32_t g = (cos(60.0f * y) * 0.5f + 0.5f) * 0xFF;
			//	uint32_t b = 0;
			//	currColor |= r << 0;
			//	currColor |= g << 8;
			//	currColor |= b << 16;
			//	return currColor;
			//};
			//gPlaceholderTexture.CreateTextureUsingGenerator(256, 256, gen);
			gPlaceholderTexture.CreateSolidColorTexture(Vec4(1));
		}

		void Shutdown()
		{
			// Clean up all containers
			for (auto containerIter : gContext.containerList)
			{
				delete containerIter.second;
			}
			gContext.containerList.clear();

			// Clean up placeholder texture
			TextureRegistry::RemoveIdFromRegistry(gPlaceholderTexture.GetId());
		}

		Vec2 GetCoordinateRelativeToTopLeft(const Vec2& coord)
		{
			Vec2 windowSize = Application::Handle->GetMainWindow()->GetSize();
			return Vec2(coord.x, windowSize.y - coord.y);
		}

	}
}