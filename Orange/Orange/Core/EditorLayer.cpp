#include "../Misc/pch.h"

#include "Application.h"
#include "DefaultBlockShader.h"
#include "EditorLayer.h"
#include "MathTypes.h"
#include "Panels/MainViewportPanel.h"
#include "../Utility/Utility.h"

using namespace Orange;

std::vector<Panel*> EditorLayer::m_panels = std::vector<Panel*>();

void EditorLayer::Initialize()
{
	// Initialize all the panels
	PanelPropertyFlags panelFlags = 0;
	panelFlags |= PanelPropertyFlags_NoDock;
	panelFlags |= PanelPropertyFlags_NoMove;
	panelFlags |= PanelPropertyFlags_NoResize;
	panelFlags |= PanelPropertyFlags_NoCollapse;
	panelFlags |= PanelPropertyFlags_NoTitleBar;

	Vec2 windowSize = Application::Handle->GetMainWindow()->GetDimensions();
	Vec2 windowPos = Application::Handle->GetMainWindow()->GetPosition();

	MainViewportPanel* leftPanel = new MainViewportPanel();
	Vec2 leftPanelSize = { 200.0f, windowSize.y * 0.8f };
	Vec2 leftPanelPos = Application::Handle->GetMainWindow()->GetPosition();
	leftPanel->Create("LeftPanel", leftPanelSize, leftPanelPos, panelFlags);

	MainViewportPanel* rightPanel = new MainViewportPanel();
	Vec2 rightPanelSize = { 200.0f, windowSize.y * 0.8f };
	Vec2 rightPanelPos = { windowPos.x + windowSize.x - rightPanelSize.x, windowPos.y };
	rightPanel->Create("RightPanel", rightPanelSize, rightPanelPos, panelFlags);

	MainViewportPanel* bottomPanel = new MainViewportPanel();
	Vec2 bottomPanelSize = { windowSize.x, ceil(windowSize.y * 0.2f) };
	Vec2 bottomPanelPos = { windowPos.x, windowPos.y + (windowSize.y * 0.8f) };
	bottomPanel->Create("BottomPanel", bottomPanelSize, bottomPanelPos, panelFlags);

	MainViewportPanel* mainViewportPanel = new MainViewportPanel();
	Vec2 mainViewportPanelSize = { windowSize.x - leftPanelSize.x - rightPanelSize.x, windowSize.y - bottomPanelSize.y };
	Vec2 mainViewportPanelPos = { windowPos.x + leftPanelSize.x, windowPos.y };
	mainViewportPanel->Create("MainViewportPanel", mainViewportPanelSize, mainViewportPanelPos, panelFlags);

	// Push back all the panels
	m_panels.push_back(leftPanel);
	m_panels.push_back(rightPanel);
	m_panels.push_back(bottomPanel);
	m_panels.push_back(static_cast<Panel*>(mainViewportPanel));
}

void EditorLayer::Shutdown()
{
	// Delete all the panels
	for (uint32_t i = 0; i < m_panels.size(); i++)
	{
		delete m_panels[i];
	}
	m_panels.clear();
}

void EditorLayer::AddPanel(Panel* panel)
{
#if defined(_DEBUG)
	for (uint32_t i = 0; i < m_panels.size(); i++)
	{
		if (m_panels[i]->GetName() == panel->GetName())
		{
			OG_LOG_WARNING("Attempting to add a panel with the same name \"%s\"", panel->GetName().c_str())
		}
	}
#endif

	m_panels.push_back(panel);
}

void EditorLayer::RemovePanel(Panel* panel)
{
	for (uint32_t i = 0; i < m_panels.size(); i++)
	{
		if (m_panels[i]->GetName() == panel->GetName())
		{
			m_panels.erase(m_panels.begin() + i);
			return;
		}
	}

	OG_LOG_WARNING("Could not remove panel with name \"%s\"", panel->GetName().c_str());
}

Panel* EditorLayer::GetPanel(const std::string& name)
{
	for (uint32_t i = 0; i < m_panels.size(); i++)
	{
		if (m_panels[i]->GetName() == name)
		{
			return m_panels[i];
		}
	}

	OG_LOG_WARNING("Could not find panel with name \"%s\"", name.c_str());
	return nullptr;
}

void EditorLayer::Update(const float& dt)
{
	UNUSED(dt);
	// Update the current panels
	DrawPanels();
}

void EditorLayer::DrawPanels()
{
	for (auto panel : m_panels)
	{
		panel->Render();
	}
}

