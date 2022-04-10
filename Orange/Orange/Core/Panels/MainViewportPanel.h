#ifndef _MAINVIEWPORTPANEL_H
#define _MAINVIEWPORTPANEL_H

#include "Panel.h"
#include "../Texture2D.h"

class MainViewportPanel : public Panel
{
public:

	MainViewportPanel();
	MainViewportPanel(const MainViewportPanel& other) = delete;
	~MainViewportPanel() = default;

	void SetTexture(void* data, const Orange::Vec2& dimensions);
	void Draw() override;

private:

	Texture2D m_texture;

};

#endif