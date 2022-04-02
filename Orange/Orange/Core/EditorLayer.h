#ifndef _PANEL_MANAGER_H
#define _PANEL_MANAGER_H

#include <unordered_map>

#include "Layer.h"

class Panel;

class EditorLayer : public Layer
{
public:

	void Initialize();
	void Shutdown();

	void AddPanel(Panel* panel);
	void RemovePanel(Panel* panel);
	Panel* GetPanel(const std::string& name);

	void Update(const float& dt);

	void BeginFrame();
	void EndFrame();

private:

	void DrawPanels();

	std::vector<Panel*> m_panels;

};

#endif