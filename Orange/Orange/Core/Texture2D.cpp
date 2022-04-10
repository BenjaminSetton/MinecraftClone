#include "../Misc/pch.h"

#include "Texture2D.h"

Texture2D::Texture2D() 
{
}

void Texture2D::Create(void* data, const Orange::Vec2& dimensions)
{
	m_data = data;
	m_dimensions = dimensions;
}

Orange::Vec2 Texture2D::GetDimensions() const { return m_dimensions; }

void Texture2D::OverridePlease()
{
	// Does nothing
}
