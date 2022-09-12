
#include "UIElement.h"

namespace Orange
{

	UIElementType UIElement::GetType() const { return UIElementType::INVALID; }

	const Vec2 UIElement::GetPosition() const { return m_position; }
	const Vec2 UIElement::GetSize() const { return m_size; }
	const Vec4 UIElement::GetColor() const { return m_color; }

	const uint32_t UIElement::GetLayerNumber() const { return m_layerNumber; }
	void UIElement::SetLayerNumber(const uint32_t layerNumber) { m_layerNumber = layerNumber; }

	void UIElement::SetPosition(const Vec2 position) { m_position = position; }
	void UIElement::SetSize(const Vec2 size) { m_size = size; }
	void UIElement::SetColor(const Vec4 color) { m_color = color; }

}