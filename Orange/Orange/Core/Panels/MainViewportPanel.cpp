#include "../../Misc/pch.h"

#include "MainViewportPanel.h"
#include "../MathTypes.h"
#include "../../Utility/Utility.h"

using namespace Orange;

MainViewportPanel::MainViewportPanel() : Panel()
{

}

void MainViewportPanel::SetTexture(void* data, const Vec2& dimensions)
{
	if (data == nullptr)
	{
		OG_LOG_WARNING("Setting null texture data on main viewport panel!");
	}
	m_texture.Create(data, dimensions);
}

void MainViewportPanel::Draw()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

	ImGui::Begin(m_name.c_str(), &m_isOpen, m_flags);

	//ImGui::ShowStyleEditor();

	Vec2 textureSize = m_texture.GetDimensions();
	ImGui::Image(m_texture.GetData(), ImVec2(textureSize.x, textureSize.y));

	ImGui::End();

	ImGui::PopStyleVar();
}