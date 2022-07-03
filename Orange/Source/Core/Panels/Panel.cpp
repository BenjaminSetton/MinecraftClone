#include "../../Misc/pch.h"

#include "../../Utility/MathTypes.h"
#include "Panel.h"

using namespace Orange;

void Panel::Create(const std::string& name, const Vec2 dimensions, const Vec2 pos, const PanelPropertyFlags flags)
{
	m_name = name;
	m_dimensions = dimensions;
	m_pos = pos;
	m_flags = flags;
	m_isOpen = true;
}

void Panel::Shutdown()
{
	// Nothing to do here at the moment
}

void Panel::Draw()
{
	ImVec2 windowSize = ImVec2(m_dimensions.x, m_dimensions.y);
	ImVec2 windowPos = ImVec2(m_pos.x, m_pos.y);
	ImGuiCond condition = ImGuiCond_Once;

	ImGui::SetNextWindowSize(windowSize, condition);
	ImGui::SetNextWindowPos(windowPos, condition);

	if (m_components.size() > 0)
	{
		// Draw all the panel's components
		for (uint32_t i = 0; i < m_components.size(); i++)
		{
			m_components[i]->Draw();
		}
	}
	else // just draw an empty window
	{
		ImGui::Begin(m_name.c_str(), &m_isOpen, m_flags);
		ImGui::Text("This is the %s", m_name.c_str());
		ImGui::End();
	}

}

const std::string& Panel::GetName() const { return m_name; }

bool Panel::IsOpen() const { return m_isOpen; }

Orange::Vec2 Panel::GetDimensions() const { return m_dimensions; }

Orange::Vec2 Panel::GetPos() const { return m_pos; }

PanelPropertyFlags Panel::GetPropertyFlags() const { return m_flags; }

bool Panel::HasComponent(const PanelComponentType type)
{
	for (uint32_t i = 0; i < m_components.size(); i++)
	{
		if (m_components[i]->GetType() == type) return true;
	}

	return false;
}

void Panel::AddComponent(PanelComponent* comp)
{
	m_components.push_back(comp);
}

void Panel::RemoveComponent(PanelComponent* comp)
{
	for (uint32_t i = 0; i < m_components.size(); i++)
	{
		if (m_components[i] == comp) m_components.erase(m_components.begin() + i);
	}

	OG_LOG_WARNING("Could not remove panel component");
}

PanelComponent* Panel::GetComponent(const PanelComponentType type)
{
	for (uint32_t i = 0; i < m_components.size(); i++)
	{
		if (m_components[i]->GetType() == type) return m_components[i];
	}

	return nullptr;
}

uint32_t Panel::GetID() const
{
	return static_cast<uint32_t>(ImGui::GetID(m_name.c_str()));
}
