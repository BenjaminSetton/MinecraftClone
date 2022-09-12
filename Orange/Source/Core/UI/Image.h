#ifndef _IMAGE_H
#define _IMAGE_H

#include "UIElement.h"

namespace Orange
{
	class Image : public UIElement
	{
	public:

		UIElementType GetType() const override { return UIElementType::IMAGE; }

	};
}


#endif