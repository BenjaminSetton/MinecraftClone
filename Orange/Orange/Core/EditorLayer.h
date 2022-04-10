#ifndef _PANEL_MANAGER_H
#define _PANEL_MANAGER_H

#include <unordered_map>

#include "Layer.h"
#include "Panels/Panel.h"

class EditorLayer : public Layer
{
public:

	EditorLayer() = delete;
	EditorLayer(const EditorLayer& other) = delete;
	~EditorLayer() = delete;

	static void Initialize();
	static void Shutdown();

	static void AddPanel(Panel* panel);
	static void RemovePanel(Panel* panel);
	static Panel* GetPanel(const std::string& name);

	static void Update(const float& dt);

private:

	static void DrawPanels();

	static std::vector<Panel*> m_panels;

};

#endif