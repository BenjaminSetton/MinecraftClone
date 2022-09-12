#include "../Misc/pch.h"

#include "Application.h"
#include "DefaultBlockShader.h"
#include "EditorLayer.h"
#include "../Utility/MathTypes.h"
#include "Panels/MainViewportPanel.h"
#include "UI/UIHelper.h"
#include "../Utility/Utility.h"

using namespace Orange;

Panel* EditorLayer::m_panels[EditorLayer_NumberOfPanels] = { nullptr, };

// TEMP TEST
bool EditorLayer::tempTest = false;
float EditorLayer::tempTest2 = 0.6f;

void EditorLayer::Initialize()
{
	Vec2 windowSize = Application::Handle->GetMainWindow()->GetSize();
	Vec2 windowPos = Application::Handle->GetMainWindow()->GetPosition();

	// Initialize all the panels
	PanelPropertyFlags panelFlags = 0;
	//panelFlags |= PanelPropertyFlags_NoDock;
	//panelFlags |= PanelPropertyFlags_NoMove;
	//panelFlags |= PanelPropertyFlags_NoResize;
	panelFlags |= PanelPropertyFlags_NoCollapse;
	panelFlags |= PanelPropertyFlags_NoTitleBar;

	//Panel* windowPanel = new Panel();
	//windowPanel->Create("WindowPanel", windowSize, { 0.0f, 0.0f }, panelFlags);

	Panel* leftPanel = new Panel();
	Vec2 leftPanelSize = { 200.0f, windowSize.y * 0.8f };
	Vec2 leftPanelPos = Application::Handle->GetMainWindow()->GetPosition();
	leftPanel->Create("LeftPanel", leftPanelSize, leftPanelPos, panelFlags);

	Panel* rightPanel = new Panel();
	Vec2 rightPanelSize = { 200.0f, windowSize.y * 0.8f };
	Vec2 rightPanelPos = { windowPos.x + windowSize.x - rightPanelSize.x, windowPos.y };
	rightPanel->Create("RightPanel", rightPanelSize, rightPanelPos, panelFlags);

	Panel* bottomPanel = new Panel();
	Vec2 bottomPanelSize = { windowSize.x, ceil(windowSize.y * 0.2f) };
	Vec2 bottomPanelPos = { windowPos.x, windowPos.y + (windowSize.y * 0.8f) };
	bottomPanel->Create("BottomPanel", bottomPanelSize, bottomPanelPos, panelFlags);

	Panel* mainViewportPanel = new Panel();
	Vec2 mainViewportPanelSize = { windowSize.x - leftPanelSize.x - rightPanelSize.x, windowSize.y - bottomPanelSize.y };
	Vec2 mainViewportPanelPos = { windowPos.x + leftPanelSize.x, windowPos.y };
	mainViewportPanel->Create("MainViewportPanel", mainViewportPanelSize, mainViewportPanelPos, panelFlags);

	// Create all the panel components and attach them to the panels
	MainViewportPanel* viewportPanelComponent = new MainViewportPanel(mainViewportPanel);
	mainViewportPanel->AddComponent(viewportPanelComponent);

	// Push back all the panels
	m_panels[static_cast<int>(PanelLocation::LEFT)] = leftPanel;
	m_panels[static_cast<int>(PanelLocation::RIGHT)] = rightPanel;
	m_panels[static_cast<int>(PanelLocation::BOTTOM)] = bottomPanel;
	m_panels[static_cast<int>(PanelLocation::CENTER)] = mainViewportPanel;
}

void EditorLayer::Shutdown()
{
	// Delete all the panels
	for (uint32_t i = 0; i < EditorLayer_NumberOfPanels; i++)
	{
		delete m_panels[i];
		m_panels[i] = nullptr;
	}
}

Panel* EditorLayer::GetPanel(const PanelLocation whichPanel)
{
	return m_panels[static_cast<int>(whichPanel)];
}

void EditorLayer::Update(const float& dt)
{
	UNUSED(dt);

	// TEMP - PLEASE REMOVE
	static float totalTime = 0.0f;
	totalTime += dt;

	UI::Begin("Debug Container");
	UI::Text("Hola ratilla sexy mira mira ocserba. Ahora hay un title-bar (sin texto todavia) y lo puedo mover y highlightear :D");
	UI::Text("Metiendo otra pinga aqui a ver que");
	UI::Checkbox(&tempTest, "Testing new checkbox! %i", (int)tempTest);
	UI::Slider(&tempTest2, 0.0f, 10.0f, "Pee-pee length", tempTest2);
	UI::End();
}

void EditorLayer::Draw()
{
	// Create a dock space over the entire viewport so we can dock all the panels. This means that
	// resizing any panel will be taken care of by ImGui
	//ImGuiID mainViewportID = ImGui::DockSpaceOverViewport(nullptr, ImGuiDockNodeFlags_PassthruCentralNode);
	//ImGui::DockBuilderAddNode(m_panels[static_cast<int>(PanelLocation::LEFT)]->GetID(), ImGuiDockNodeFlags_None);

	// Comment out for now, so I can test my font renderer
	//for (auto panel : m_panels)
	//{
	//	panel->Draw();
	//}
}

