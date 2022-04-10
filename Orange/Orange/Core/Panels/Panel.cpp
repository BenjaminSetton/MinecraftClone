#include "../../Misc/pch.h"

#include "../MathTypes.h"
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

void Panel::Render()
{
	ImVec2 windowSize = ImVec2(m_dimensions.x, m_dimensions.y);
	ImVec2 windowPos = ImVec2(m_pos.x, m_pos.y);
	ImGuiCond condition = ImGuiCond_Once;

	ImGui::SetNextWindowSize(windowSize, condition);
	ImGui::SetNextWindowPos(windowPos, condition);

	// Allow derived classes to add their own stuff to the panels
	Draw();
}

const std::string& Panel::GetName() const { return m_name; }

Orange::Vec2 Panel::GetDimensions() const { return m_dimensions; }

Orange::Vec2 Panel::GetPos() const { return m_pos; }

PanelPropertyFlags Panel::GetPropertyFlags() const { return m_flags; }