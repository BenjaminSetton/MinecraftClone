#ifndef _TEXT_H
#define _TEXT_H

#include "UIElement.h"

namespace Orange
{
	class Text : public UIElement
	{
	public:

		UIElementType GetType() const override { return UIElementType::TEXT; }

	};
}


#endif