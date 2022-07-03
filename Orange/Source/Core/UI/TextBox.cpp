
#include "TextBox.h"

namespace Orange
{

	TextBox::TextBox()
		: m_text(""), m_size(Vec2(50, 0)), m_padding(3, 3)
	{
		m_layerNumber = 0;
	}

	TextBox::TextBox(const std::string text, const Vec2 size /*= Vec2(50, 0)*/, const Vec2 padding /*= Vec2(3, 3)*/)
		: m_text(text), m_size(size), m_padding(padding)
	{
		m_layerNumber = 0;
	}

	TextBox::TextBox(const TextBox& other)
	{
		m_text = other.m_text;
		m_size = other.m_size;
		m_padding = other.m_padding;

		m_layerNumber = other.m_layerNumber;
	}

	TextBox::~TextBox()
	{
	}

	const std::string& TextBox::GetText()
	{
		return m_text;
	}

	const Vec2 TextBox::GetSize()
	{
		return m_size;
	}

	const Vec2 TextBox::GetPadding()
	{
		return m_padding;
	}

	void TextBox::SetText(const std::string text)
	{
		m_text = text;
	}

	void TextBox::AppendText(const std::string text)
	{
		m_text.append(text);
	}

	void TextBox::ClearText()
	{
		m_text.clear();
	}

	void TextBox::SetDrawOutline(const bool drawOutline)
	{
		m_drawOutline = drawOutline;
	}

	const Orange::UIComponentType TextBox::GetType() const
	{
		return UIComponentType::TEXTBOX;
	}

	void TextBox::Draw() const
	{
		// TODO - Define how to draw a textbox
		if (m_drawOutline)
		{
			// Submit the four lines that make up the 
		}


	}

}


