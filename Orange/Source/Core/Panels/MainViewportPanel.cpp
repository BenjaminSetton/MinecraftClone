#include "../../Misc/pch.h"

#include "MainViewportPanel.h"
#include "../../Utility/MathTypes.h"
#include "../../Utility/Utility.h"

namespace Orange
{

	MainViewportPanel::MainViewportPanel(Panel* parent)
	{
		m_type = PanelComponentType::MAIN_VIEWPORT_PANEL;
		m_parent = parent;
	}

	void MainViewportPanel::SetTexture(void* data, const Vec2& size)
	{
		if (!data)
		{
			OG_LOG_WARNING("Setting null texture data on main viewport panel!");
		}

		TextureData texData;
		texData.format = TextureFormat::RGBA_32;
		texData.dimensions = TextureDimensions::TWO;
		texData.size = { size.x, size.y, 0.0f };
		texData.data = data;
		m_texture.SetReferenceDataToTexture(texData);
	}

	void MainViewportPanel::Draw()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin(m_parent->GetName().c_str(), nullptr, m_parent->GetPropertyFlags());

		TextureData texData = m_texture.GetTextureData();
		ImGui::Image(texData.data, ImVec2(texData.size.x, texData.size.y));

		ImGui::End();
		ImGui::PopStyleVar();
	}

}
