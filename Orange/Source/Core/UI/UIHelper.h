#ifndef _UIHELPER_H
#define _UIHELPER_H

#include "UITypes.h"

namespace Orange
{
	// Forward declaration
	struct Vec2;

	// This UI namespace will include all the public API that the user will be able to interact with
	namespace UI
	{
		const UIContextDescription* GetContextDescriptionRO();

		UIContextDescription* GetContextDescription();

		void Text(const char* format, ...);

		void Image(void* data, const Vec2 size);
	}
}

#endif