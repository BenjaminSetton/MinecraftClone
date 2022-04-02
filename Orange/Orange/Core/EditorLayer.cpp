#include "../Misc/pch.h"

#include "EditorLayer.h"
#include "Panel.h"
#include "../Utility/Utility.h"

void EditorLayer::Initialize()
{
	// Initialize all the panels
	PanelPropertyFlags panelFlags = 0;
	//panelFlags |= PanelPropertyFlags_No_Close;
	//panelFlags |= PanelPropertyFlags_No_Dock;
	//panelFlags |= PanelPropertyFlags_No_Move;
	//panelFlags |= PanelPropertyFlags_No_Resize;

	Panel* leftPanel = new Panel();
	leftPanel->Create("LeftPanel", 100, 100, 0, 0, panelFlags);

	Panel* rightPanel = new Panel();
	rightPanel->Create("RightPanel", 100, 100, 0, 0, panelFlags);

	// Push back all the panels
	m_panels.push_back(leftPanel);
	m_panels.push_back(rightPanel);
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
		panel->Draw();
	}
}

