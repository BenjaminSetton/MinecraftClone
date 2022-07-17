#ifndef _UITYPES_H
#define _UITYPES_H

#include "../Texture.h"

// This file will contain a lot of enums / structures that will define how the engine's UI is stored / represented
namespace Orange
{

	static Vec2 QUAD_COORDINATES[6] =
	{
		Vec2(1, 1), // BR
		Vec2(1, 0), // TR
		Vec2(0, 0), // TL
		Vec2(0, 1), // BL
		Vec2(1, 1), // BR
		Vec2(0, 0), // TL
	};

	enum class UIElementType : uint16_t
	{
		INVALID = 0,
		TEXT,
		IMAGE
		// TODO - Add more
	};

	// Provides context around the draw calls in the current frame
	struct UIContextDescription
	{
		Vec3 colorMultiplier = Vec3(1.0f, 1.0f, 1.0f);
	};

	// The draw command struct that will get filled out upon any calls to UI::XXXX() calls. This is a description of what
	// we want to draw, and it's up to the UIBuffer to create the correct quad vertices. Then the UIRenderer will request
	// those vertices and handle all the graphics API calls from there (eg updating buffers, setting up shaders, drawing, etc)
	struct UIDrawCommand
	{
		UIElementType type = UIElementType::INVALID;

		// A handle to an existing texture. We don't own it, just reference it
		Texture* textureHandle	= nullptr;

		union UIDrawContext
		{
			struct Text
			{
				char c;
			} text;

			struct Image
			{
			} image;
		} drawContext;
	};

	// We define a vertex with a single coordinate that combines both the position and UV coordinate. When we pass the size and position
	// of the quad to the vertex shader, we can extract the screen-space position by multiplying by the size and adding the position (since the 'coordinate'
	// variable is normalized).
	struct UIVertex
	{
		Vec2 coordinate		= Vec2(0, 0);
		Vec2 size			= Vec2(0, 0);
		Vec2 position		= Vec2(0, 0);
		Vec3 color			= Vec3(0, 0, 0);
	};

}

#endif