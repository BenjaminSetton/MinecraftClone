#ifndef UICOMPONENT_H
#define UICOMPONENT_H

#include "UITypes.h"

namespace Orange
{

	// This virtual class defines what a derived class has to implement to become
	// a well-defined, drawable UI element. It will also include some utility functions,
	// such as layer number.
	class UIElement
	{
	public:

		virtual const UIElementType GetType() const { return UIElementType::INVALID; }
		virtual void Draw() const { return; }

		const Vec2 GetPosition() const { return m_position; };
		const Vec2 GetSize() const { return m_size; };
		const Vec2 GetPadding() const { return m_padding; };
		const Vec3 GetColor() const { return m_color; }

		// Public non-virtual functions
		const uint32_t GetLayerNumber() const { return m_layerNumber; }
		void SetLayerNumber(const uint32_t layerNumber) { m_layerNumber = layerNumber; }

		// This function is overridden so that the UIManager can use a priority queue to sort
		// the elements by layer and draw them!
		bool operator<=(const UIElement& other)
		{
			return GetLayerNumber() <= other.GetLayerNumber();
		}

	protected:

		void SetPosition(const Vec2 position) { m_position = position; };
		void SetSize(const Vec2 size) { m_size = size; };
		void SetPadding(const Vec2 padding) { m_padding = padding; };
		void SetColor(const Vec3 color) { m_color = color; }

		Vec2 m_position;
		Vec2 m_size;
		Vec2 m_padding;
		Vec3 m_color;
		uint32_t m_layerNumber;

	};
}


#endif