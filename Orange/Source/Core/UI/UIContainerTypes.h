#ifndef _UICONTAINERTYPES_H
#define _UICONTAINERTYPES_H

#include <queue>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

#include "../Texture.h"
#include "../../Utility/MathTypes.h"
#include "UIDataTypes.h"
#include "UIElement.h"
#include "../../Utility/Utility.h"

namespace Orange
{
	// Typedef for descriptive types
	typedef uint64_t UIHash;
	constexpr UIHash INVALID_UIHASH = 0;

	// The draw command struct that will get filled out upon any calls to UI::XXXX() calls. This is a description of what
	// we want to draw, and it's up to the UIBuffer to create the correct quad vertices. Then the UIRenderer will request
	// those vertices and handle all the graphics API calls from there (e.g. updating buffers, setting up shaders, drawing, etc)
	struct UIDrawCommand
	{
		Vec3 colorModifier = Vec3(1.0f, 1.0f, 1.0f);

		// A handle to an existing texture. We don't own it, just reference it
		Texture textureHandle = Texture();

		//The renderer needs to know what to draw, so this needs to be here
		UIElementType type = UIElementType::INVALID;

		float roundEdgesMultiplier = 0.0f;

		union UIDrawContext
		{
			struct Text
			{
				char c;
			} text;

			struct Image
			{
			} image;
		} drawContext = {};

#ifdef OG_DEBUG
#define DEBUG_DRAW_COMMAND_FILE_AND_LINE(objectName) \
objectName.fileName = __FILENAME__; \
objectName.lineNumber = __LINENUMBER__;

		const char* fileName = "";
		int32_t lineNumber = 0;
#else
#define DEBUG_DRAW_COMMAND_FILE_AND_LINE
#endif
	};

	static const Vec4 defaultBackgroundColor = Vec4(0.49f, 0.55f, 0.59f, 0.70f);
	static const Vec4 selectedBackgroundColor = Vec4(0.49f / 1.4f, 0.55f / 1.4f, 0.59f / 1.4f, 0.70f);
	static const Vec4 titleBarColor = Vec4(0.03f, 0.05f, 0.17f, 1.0f);
	static const Vec4 checkBoxCheckedColor = Vec4(0.0f, 1.0f, 0.0f, 1.0f);
	static const Vec4 checkBoxUncheckedColor = Vec4(1.0f, 0.0f, 0.0f, 1.0f);
	static const Vec4 sliderBarColor = titleBarColor;
	static const Vec4 sliderHandleUnselectedColor = titleBarColor * 4.0f;
	static const Vec4 sliderHandleSelectedColor = titleBarColor * 3.0f;

	enum DataFlags : uint32_t
	{
		UIContainer_CanHighlight				= OG_BIT(0),
		UIContainer_CanCursorLock				= OG_BIT(1),
		UIContainer_KeepInWindowBounds			= OG_BIT(2),
		UIContainer_ScrollAllowed				= OG_BIT(3),
		UIContainer_WrapText					= OG_BIT(4),
		UIContainer_ShowTitleBar				= OG_BIT(5)
	};


	// A UIContainer is a container of UIElements with special attributes. A UIElement does not have a UIRect, and this is intentional, because
	// I want to be able to interact specifically with containers, not every single UIElement by itself. An example of interacting with UIElements
	// individually would be having the ability to click on and drag each individual character from a string of text, for example.
	class UIContainer
	{
	public:

		UIContainer()
		{
			dataFlags								= UIContainer_CanHighlight | UIContainer_CanCursorLock | UIContainer_WrapText | UIContainer_ShowTitleBar;
			containerRect							= UIRect();
			remainingSpace							= containerRect;
			effectiveSize							= Vec2(0);
			padding									= Vec2(5);
			color									= defaultBackgroundColor;
			titleBarSize							= Vec2(containerRect.GetSize().x, 25.0f);
			titleBarRect							= UIRect();
			hash									= INVALID_UIHASH;

			// DO NOT USE
			backgroundTextureObject.CreateSolidColorTexture(Vec4(1.0f, 1.0f, 1.0f, 1.0f));
		}

		typedef uint32_t UIContainerFlags;

	public:

		const UIRect GetRemainingSpace() const
		{
			return remainingSpace;
		}

		const Vec2 GetRemainingSize() const
		{
			return remainingSpace.GetSize();
		}

		const Vec2 GetAbsoluteSize() const
		{
			return containerRect.GetSize();
		}

		const Vec2 GetEffectiveSize() const
		{
			return effectiveSize;
		}

		bool RequiresScrollBar() const
		{
			return ((GetEffectiveSize().y > GetAbsoluteSize().y) || (GetEffectiveSize().x > GetAbsoluteSize().x));
		}

		const bool IsDataFlagSet(const DataFlags& flag) const
		{
			return static_cast<bool>(dataFlags & flag);
		}

		void SetDataFlag(const DataFlags& flag)
		{
			dataFlags |= flag;
		}

		void ClearDataFlag(const DataFlags& flag)
		{
			dataFlags &= ~flag;
		}

		void InvalidateSpace(const Vec2& spaceUsed)
		{
			Vec2 sizeToExtent = spaceUsed / 2.0f;
			remainingSpace.extent -= sizeToExtent;
			remainingSpace.center -= sizeToExtent;

			// Increment the effective size of the container
			effectiveSize += spaceUsed;
		}

		void ResetDynamicData()
		{
			remainingSpace = containerRect;
			effectiveSize = Vec2(0);
		}

		// The size of the title bar is in pixels, with a max width of the container's
		// width and a maximum height of the height of the container
		Vec2 titleBarSize;

		// This flag allows the container to be locked to the cursor.
		// This is used when a container is being moved around
		//bool cursorLocked;

		UIContainerFlags dataFlags;

		// Contains information about the absolute size and position of the container (e.g. what we'll render and where)
		UIRect containerRect;

		// Contains information about how much space is left for other "stuff" to use. For example, when we append text
		// to this container we invalidate some space at the top of the containerRect, but we keep track of it in here
		UIRect remainingSpace;

		// The rect that represents the titlebar
		UIRect titleBarRect;

		// The hash of this container (guaranteed to be a UUID)
		UIHash hash;

		// Represents the effective size of the container, which is not always equal to the physical size
		// of the UIRect. If the children take up more space than what the UIRect can represent, the container
		// becomes scrollable. The allowScroll flag determines whether we allow a scrolling for this container
		Vec2 effectiveSize;

		// The horizontal and vertical padding applied before any component is rendered
		Vec2 padding;

		// The background color of the container
		Vec4 color;

		// Distance from center of container to the cursor if we're cursor locked. Used for dragging
		//Vec2 distToCursorIfCursorLocked;

		// DO NOT USE
		Texture backgroundTextureObject;

	};

	// We define a vertex with a single coordinate that combines both the position and UV coordinate. When we pass the size and position
	// of the quad to the vertex shader, we can extract the screen-space position by multiplying by the size and adding the position (since the 'coordinate'
	// variable is normalized).
	struct UIVertex
	{
		Vec2 coordinate = Vec2(0.0f, 0.0f);
		Vec2 size = Vec2(0.0f, 0.0f);
		Vec2 position = Vec2(0.0f, 0.0f);
		Vec4 color = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
	};


	// Provides context around the draw calls in the current frame
	struct UIContext
	{
		std::unordered_map<UIHash, UIContainer*> containerList = std::unordered_map<UIHash, UIContainer*>();

		// Keeps track of the "depth" of every container. This is updated every time a Begin()
		// call is made, and is reset at the beginning of each frame. The index corresponds to the depth
		// value of the hash, and the higher the Z value the "higher" it is on the screen. Top-most container
		// is (ZMap.size() - 1)
		std::vector<UIHash> ZMap = std::vector<UIHash>();

		// Stores the container that all UI::XXXX calls will append their data to. A container is pushed every time
		// UI::Begin() is called, and popped every time UI::End() is called. By definition, the container inside the stack
		// should always point to the last element inside the containerList
		std::stack<UIContainer*> containerStack = std::stack<UIContainer*>();

		// Rendering-related data members
		std::queue<UIDrawCommand> drawCommandList = std::queue<UIDrawCommand>();
		std::queue<UIVertex> vertexList = std::queue<UIVertex>();

		// Slider data
		Vec2 sliderBarSizeInPixels					= Vec2(150, 20);
		Vec2 sliderHandleSizeInPixels				= Vec2(12, static_cast<int>(sliderBarSizeInPixels.y));

		UIHash activeID								= INVALID_UIHASH;	// Hash of the widget that is currently active
		UIHash prevActiveID							= INVALID_UIHASH;	// Hash of the previous frame, equal to current active ID if the same widget is still active
		float activeIDTimer							= 0.0f;				// Timer for how long the active widget has been active for
		UIHash hoveredID							= INVALID_UIHASH;	// Hash of the widget that is currently hovered
		UIHash prevHoveredID						= INVALID_UIHASH;	// Hash of the previous frame, equal to current hovered ID if the same widget is still hovered
		float hoveredIDTimer						= 0.0f;				// Timer for how long the hovered widget has been hovered for
		UIHash parentID								= INVALID_UIHASH;	// Hash of the container that every widget call is going to add itself to
		uint32_t maxDepth							= 0;				// Keeps count of the max depth of any given container. This allows us to determine which container is on top of which other one
		Vec2 distToCenterOfActiveContainer			= Vec2(0);			// Stores the distance to the center of the active container for dragging
		const char* sliderDecimalPrecisionFormat	= "%0.3f";			// The format that the slider uses to display it's interpolated value
	};
}

#endif