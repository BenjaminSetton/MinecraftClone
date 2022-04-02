#include "../Misc/pch.h"

#include "Application.h"
#include "Panel.h"

void Panel::Create(const std::string& name, const int32_t width, const int32_t height, const int32_t x, const int32_t y, const PanelPropertyFlags flags)
{
	m_name = name;
	m_width = width;
	m_height = height;
	m_x = x;
	m_y = y;
	m_flags = flags;
	isOpen = true;
}

void Panel::Shutdown()
{
	// Nothing to do here at the moment
}

void Panel::Draw()
{
	ImVec2 windowSize = ImVec2(static_cast<float>(m_width), static_cast<float>(m_height));
	ImVec2 windowPos = ImVec2(static_cast<float>(Application::Handle->GetMainWindow()->GetX() + m_x), static_cast<float>(Application::Handle->GetMainWindow()->GetY() + m_y));
	ImGuiCond condition = ImGuiCond_Once;

	ImGui::SetNextWindowSize(windowSize, condition);
	ImGui::SetNextWindowPos(windowPos, condition);

	if ((m_flags & PanelPropertyFlags_No_Close) == 0)
	{
		ImGui::Begin(m_name.c_str(), nullptr, m_flags);
	}
	else
	{
		ImGui::Begin(m_name.c_str(), &isOpen, m_flags);
	}

	ImGui::Text("Testing panel %s", m_name.c_str());


	ImGui::End();
}

const std::string& Panel::GetName() const { return m_name; }

int32_t Panel::GetWidth() const { return m_width; }

int32_t Panel::GetHeight() const { return m_height; }

int32_t Panel::GetXPosition() const { return m_x; }

int32_t Panel::GetYPosition() const { return m_y; }

PanelPropertyFlags Panel::GetPropertyFlags() const { return m_flags; }

Panel* Panel::GetParent() const { return m_parent; }

const std::vector<Panel*>& Panel::GetChildren() const { return m_children; }
