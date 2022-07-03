#ifndef _TEXTBOX_H
#define _TEXTBOX_H

#include <string>
#include "../../Utility/MathTypes.h"
#include "UIComponent.h"

namespace Orange
{

	class TextBox : public UIComponent
	{
	public:

		TextBox();
		TextBox(const std::string text, const Vec2 size = Vec2(50, 0), const Vec2 padding = Vec2(3, 3));
		TextBox(const TextBox& other);
		~TextBox();

		const std::string& GetText();
		const Vec2 GetSize();
		const Vec2 GetPadding();
		void SetText(const std::string text);
		void AppendText(const std::string text);
		void ClearText();
		void SetDrawOutline(const bool drawOutline);

		const UIComponentType GetType() const override;
		void Draw() const override;

	private:

		std::string m_text;
		Vec2 m_size;
		Vec2 m_padding;
		// TODO - Add more member variables
		bool m_drawOutline;

	};
}


#endif