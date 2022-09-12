#ifndef UICOMPONENT_H
#define UICOMPONENT_H

#include "../../Utility/MathTypes.h"
#include "UIDataTypes.h"

namespace Orange
{

	// This virtual class defines what a derived class has to implement to become
	// a well-defined, drawable UI element. It will also include some utility functions,
	// such as layer number.
	class UIElement
	{
	public:

		virtual Orange::UIElementType GetType() const;

		// Public non-virtual functions
		const Vec2 GetPosition() const;
		const Vec2 GetSize() const;
		const Vec4 GetColor() const;

		const uint32_t GetLayerNumber() const;
		void SetLayerNumber(const uint32_t layerNumber);

	protected:

		void SetPosition(const Vec2 position);
		void SetSize(const Vec2 size);
		void SetColor(const Vec4 color);

		Vec2 m_position;
		Vec2 m_size;
		Vec4 m_color;
		uint32_t m_layerNumber;

	};
}


#endif