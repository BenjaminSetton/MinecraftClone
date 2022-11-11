
#include <functional>
#include <stdarg.h>
#include <sstream>

#include "../Application.h"
#include "../../Utility/FontManager.h"
#include "../../Utility/HeapOverrides.h"
#include "../../Utility/Input.h"
#include "Image.h"
#include "../../Utility/Math.h"
#include "../../Utility/MathTypes.h"
#include "Text.h"
#include "UIHelper.h"
#include "../../Utility/Utility.h"

static constexpr uint32_t MAX_TEXT_BUFFER_SIZE = 1024;

static Orange::UIContext gContext;

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
		const uint32_t GetContainerDepth(const UIHash& containerHash)
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

		UIHash FindHighestContainer()
		{
			Vec2 mousePos = Input::GetMousePositionRelativeToBottomLeft();
			UIHash highestContainer = INVALID_UIHASH;
			uint32_t maxDepth = 0;

			for (auto containerIter : gContext.containerList)
			{
				UIContainer* container = containerIter.second;

				// Allow the user to move the container by clicking on the titlebar (no titlebar = no moving around)
				if (container->titleBarRect.IsPointInRect(mousePos))
				{
					uint32_t containerDepth = GetContainerDepth(container->hash);
					if (containerDepth > maxDepth)
					{
						maxDepth = containerDepth;
						highestContainer = container->hash;
					}
				}
			}

			return highestContainer;
		}

		const UIHash GetHashForString(const char* str)
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

		void DrawQuad_Internal(const UIRect& quadRect, const Vec4& color, const Texture& tex, const UIElementType& elementType)
		{
			// Emplace the quad and draw command
			EmplaceQuadVertices(quadRect.GetMin(), quadRect.GetSize(), color);

			UIDrawCommand drawCommand;
			drawCommand.textureHandle = tex;
			drawCommand.type = elementType;
			EmplaceDrawCommand(drawCommand);
		}

		// Calculates position of text quads using parameters and enqueues the vertices and draw commands to the draw queues
		const Vec2 DrawText_Internal(const char* text, const Vec2 startPosition, const bool wrapText, const uint32_t widthLimit, const Vec2 padding, const HORIZONTAL_TEXT_ALIGNMENT& horAlign)
		{
			OG_ASSERT(padding.x < widthLimit);

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

				// Calculate how much space the text takes up, we
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
			}

			Vec2 prevPos(0);

			// Dictate where we will start writing our chars to (regarding alignment)
			switch (horAlign)
			{
			case HORIZONTAL_TEXT_ALIGNMENT::LEFT:
			{
				prevPos = padding;
				break;
			}
			case HORIZONTAL_TEXT_ALIGNMENT::CENTER:
			{
				prevPos = Vec2((widthLimit - spaceUsed.x) / 2.0f, padding.y);
				break;
			}
			case HORIZONTAL_TEXT_ALIGNMENT::RIGHT:
			{
				prevPos = Vec2(widthLimit - spaceUsed.x, padding.y);
				break;
			}
			default:
			{
				OG_ERROR("Unknown alignment");
				break;
			}
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
					if (prevPos.x + charData.bearing.x + size.x >= widthLimit - padding.x)
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

				widthTaken = min(static_cast<uint32_t>(prevPos.x + charData.bearing.x + size.x), widthLimit);

				// Text starts at top-left of the container and is bottom-aligned
				Vec2 vertexPos = Vec2
				(
					startPosition.x + startQuadPosition.x,
					startPosition.y + startQuadPosition.y - FontManager::GetFontStats().maxCharSize.y - padding.y
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
			DrawQuad_Internal(titleBarRect, titleBarColor, tex, UIElementType::CONTAINER);

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
			drawCommand.type = UIElementType::CONTAINER;
			EmplaceDrawCommand(drawCommand);

			// Return the space used by the checkbox
			return checkBoxSize;
		}

		const Vec2 DrawSlider_Internal(const UIRect& sliderBarRect, const UIRect& sliderHandleRect, const Texture& tex, const bool& isMouseHovering)
		{
			// Draw the slider bar quad
			DrawQuad_Internal(sliderBarRect, sliderBarColor, tex, UIElementType::CONTAINER);

			// Draw the slider handle quad
			DrawQuad_Internal(sliderHandleRect, isMouseHovering ? sliderHandleSelectedColor : sliderHandleUnselectedColor, tex, UIElementType::CONTAINER);

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
			Vec2 spaceUsed = DrawText_Internal(tempBuffer, container->GetRemainingSpace().GetCorner(RECT_CORNER::TOP_LEFT), container->IsDataFlagSet(UIContainer_WrapText), static_cast<uint32_t>(container->GetRemainingSize().x), container->padding, HORIZONTAL_TEXT_ALIGNMENT::LEFT);
			
			// Invalidate the space we just used for the text
			container->InvalidateSpace(Vec2(0.0f, spaceUsed.y));
		}

		void Image(const Texture& tex)
		{
			UNUSED(tex);
			//// Assert that there's a container in the stack. Otherwise, this means that we are attempting to calls UI::XXXX() without
			//// a matching Begin/End around it
			//if (g_Context.containerStack.size() == 0)
			//{
			//	OG_ASSERT_MSG(false, "Attempting to call UI::Image() without calling UI::Begin() first!");
			//	return;
			//}

			//// Get the container we're appending to
			//// NOTE - We'll use this soon enough
			////UIContainer* container = g_Context.containerStack.top();

			//// Create the 6 corresponding vertices for the quad and send to UIBuffer
			//Vec2 position = Vec2(100, 250);

			//// NOTE: This should be controlled by some sort of style var that gets pushed onto a stack and popped
			//// Exactly like how ImGui handles this type of thing. Hard-coded to white for now
			//Vec4 colorModifier = { 1.0f, 1.0f, 1.0f, 1.0f };

			//EmplaceQuadVertices(position, size, colorModifier);

			//UIDrawCommand drawCommand; DEBUG_DRAW_COMMAND_FILE_AND_LINE(drawCommand)
			//drawCommand.textureHandle = tex;
			//drawCommand.type = UIElementType::IMAGE;

			//g_Context.drawCommandList.emplace(drawCommand);
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
			Vec2 checkBoxStartingPos = Vec2(container->GetRemainingSpace().GetCorner(RECT_CORNER::TOP_LEFT).x + container->padding.x + checkBoxExtent.x, container->GetRemainingSpace().GetCorner(RECT_CORNER::TOP_LEFT).y - ((FontManager::GetVerticalOffsetForLineNumber(1) + container->padding.y) / 2.0f));
			
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
			DrawCheckbox_Internal(checkBox.GetMin(), checkBoxExtent * 2.0f, container->backgroundTextureObject, pBool);

			// Draw the text after the checkbox
			Vec2 textStartingPos = Vec2(container->GetRemainingSpace().GetCorner(RECT_CORNER::TOP_LEFT).x + checkBox.GetSize().x + container->padding.x, container->GetRemainingSpace().GetCorner(RECT_CORNER::TOP_LEFT).y);
			Vec2 spaceUsed = DrawText_Internal(tempBuffer, textStartingPos, container->IsDataFlagSet(UIContainer_WrapText), static_cast<uint32_t>(container->GetRemainingSize().x), container->padding, HORIZONTAL_TEXT_ALIGNMENT::LEFT);

			// Invalidate the space we just used for the text
			container->InvalidateSpace(Vec2(0.0f, spaceUsed.y));
		}

		void Slider(float* value, const float min, const float max, const char* format, ...)
		{
			// Value between 0 and 1
			float tween = ((*value) - min) / (max - min);
			Math::Clamp(tween, 0.0f, 1.0f);

			UIContainer* container = GetContainer();

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

			DrawSlider_Internal(barRect, handleRect, container->backgroundTextureObject, gContext.hoveredID == id);

			Vec2 textStartPosition = container->remainingSpace.GetCorner(RECT_CORNER::TOP_LEFT) + Vec2(barRect.GetSize().x + container->padding.x, -container->padding.y);
			DrawText_Internal(tempBuffer, textStartPosition, false, static_cast<uint32_t>(container->containerRect.GetMax().x - textStartPosition.x), container->padding.x, HORIZONTAL_TEXT_ALIGNMENT::LEFT);

			// Draw the current value of the variable
			auto string = ToString(*value);
			DrawText_Internal(string.c_str(), barRect.GetCorner(RECT_CORNER::TOP_LEFT), false, static_cast<uint32_t>(barRect.GetSize().x), container->padding.x, HORIZONTAL_TEXT_ALIGNMENT::CENTER);
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

			DrawQuad_Internal(container->containerRect, container->color, container->backgroundTextureObject, UIElementType::CONTAINER);

			// Create vertices and a draw command for the new container. We want to do this here
			// because the API allows multiple elements to be appended to a container every frame
			// with separate Begin/End calls
			//Vec2 size = container->containerRect.GetSize();
			//Vec4 color = container->color;
			//Vec2 position = container->containerRect.GetMin();
			//EmplaceQuadVertices(position, size, color);

			//UIDrawCommand drawCommand; DEBUG_DRAW_COMMAND_FILE_AND_LINE(drawCommand)
			//drawCommand.textureHandle = container->backgroundTextureObject;
			//drawCommand.type = UIElementType::CONTAINER;
			//EmplaceDrawCommand(drawCommand);

			// Append the title-bar, if necessary
			if (container->IsDataFlagSet(UIContainer_ShowTitleBar))
			{
				Vec2 titleBarExtent = container->titleBarSize / 2.0f;
				UIRect titlebarRect;
				titlebarRect.center = Vec2(container->containerRect.GetCorner(RECT_CORNER::TOP_LEFT).x + titleBarExtent.x, container->containerRect.GetCorner(RECT_CORNER::TOP_LEFT).y - titleBarExtent.y);
				titlebarRect.extent = titleBarExtent;
				Vec2 spaceUsed = DrawTitleBar_Internal(titlebarRect, container->backgroundTextureObject);

				container->titleBarRect = titlebarRect;

				// Draw the name of the container
				DrawText_Internal(containerName, container->containerRect.GetCorner(RECT_CORNER::TOP_LEFT), false, static_cast<uint32_t>(container->titleBarSize.x), container->padding, HORIZONTAL_TEXT_ALIGNMENT::LEFT);

				// Invalidate the space from the title-bar. Note that it
				// doesn't take up any extra horizontal space unlike the scroll-bar
				container->InvalidateSpace(Vec2(0.0f, spaceUsed.y));
			}
			else
			{
				container->titleBarRect = UIRect(Vec2(0), Vec2(0));
			}
			
			// Push it onto the stack
			gContext.containerStack.push(container);

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

			// Reset the active ID if the LMB is up
			if (!Input::IsMouseDown(MouseCode::LBUTTON))
			{
				gContext.activeID = INVALID_UIHASH;
				gContext.activeIDTimer = 0;
				gContext.distToCenterOfActiveContainer = Vec2(0);
			}

			Vec2 mousePos = Input::GetMousePositionRelativeToBottomLeft();

			// We're dragging the container around, so update it's position.
			// We also don't care whether we're hovering over another container
			// or w/e else might be happening, so this check should go first
			if (IsContainer(gContext.activeID) && (gContext.activeID != INVALID_UIHASH))
			{
				UIContainer* activeContainer = gContext.containerList[gContext.activeID];
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

				// Keep the color as "selected", even though the cursor could briefly move out of the container
				activeContainer->color = selectedBackgroundColor;
			}

			// Find the active and hovered container
			UIHash highestContainerHash = FindHighestContainer();

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
				if (hotContainer->IsDataFlagSet(UIContainer_CanCursorLock))
				{
					// Set the distance to the center of the container in the context
					gContext.distToCenterOfActiveContainer = hotContainer->containerRect.center - mousePos;
				}
				gContext.activeID = highestContainerHash;
			}

			// Reset the depth map
			memset(&gContext.ZMap[0], INVALID_UIHASH, gContext.ZMap.size() * sizeof(UIHash));
		}

		void Initialize()
		{
			// Do INI parsing and other necessary stuff
		}

		void Shutdown()
		{
			// Clean up all containers
			for (auto containerIter : gContext.containerList)
			{
				delete containerIter.second;
			}
			gContext.containerList.clear();
		}

	}
}