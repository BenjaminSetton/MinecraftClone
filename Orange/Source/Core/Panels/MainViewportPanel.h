#ifndef _MAINVIEWPORTPANEL_H
#define _MAINVIEWPORTPANEL_H

#include "Panel.h"
#include "PanelComponent.h"
#include "../Texture.h"

namespace Orange
{

	class MainViewportPanel : public PanelComponent
	{
	public:

		MainViewportPanel(Panel* parent);
		MainViewportPanel(const MainViewportPanel& other) = delete;
		~MainViewportPanel() = default;

		void SetTexture(void* data, const Vec2& size);
		void Draw() override;

	private:

		Texture m_texture;

	};

}


#endif