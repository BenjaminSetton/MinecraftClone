#ifndef _PANEL_MANAGER_H
#define _PANEL_MANAGER_H

#include <unordered_map>

#include "Layer.h"
#include "Panels/Panel.h"

static constexpr int32_t EditorLayer_NumberOfPanels = 4;

class EditorLayer : public Layer
{
public:

	enum class PanelLocation : int8_t
	{
		BOTTOM = 0,
		LEFT,
		RIGHT,
		CENTER
	};

	EditorLayer() = delete;
	EditorLayer(const EditorLayer& other) = delete;
	~EditorLayer() = delete;

	static void Initialize();
	static void Shutdown();

	static Panel* GetPanel(const PanelLocation whichPanel);

	static void Update(const float& dt);

	static void Draw();

	// TODO: Implement an OnResize method to adjust panel dimensions upon window resizing

private:

	static Panel* m_panels[EditorLayer_NumberOfPanels];

	static bool tempTest;
	static float tempTest2;

};

#endif