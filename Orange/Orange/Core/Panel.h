#ifndef _PANEL_H
#define _PANEL_H

#include <string>
#include <vector>

#include "../imgui/imgui.h"
#include "../Utility/Utility.h"

enum PanelPropertyFlags_
{
	PanelPropertyFlags_No_Resize	= ImGuiWindowFlags_NoResize,
	PanelPropertyFlags_No_Close		= OG_BIT(31), // This flag is special because ImGui doesn't have a specific flag for this
	PanelPropertyFlags_No_Move		= ImGuiWindowFlags_NoMove,
	PanelPropertyFlags_No_Dock		= ImGuiWindowFlags_NoDocking
};

typedef uint32_t PanelPropertyFlags;

class Panel
{
public:

	Panel() = default;
	Panel(const Panel& panel) = default;
	~Panel() = default;

	void Create(const std::string& name, const int32_t width, const int32_t height, const int32_t x, const int32_t y, const PanelPropertyFlags flags);
	void Shutdown();
	void Draw();

	const std::string& GetName() const;
	int32_t GetWidth() const;
	int32_t GetHeight() const;
	int32_t GetXPosition() const;
	int32_t GetYPosition() const;
	PanelPropertyFlags GetPropertyFlags() const;
	Panel* GetParent() const;
	const std::vector<Panel*>& GetChildren() const;

private:

	std::string m_name = "";
	int32_t m_width = 0;
	int32_t m_height = 0;
	int32_t m_x = 0;
	int32_t m_y = 0;
	PanelPropertyFlags m_flags = 0;
	bool isOpen = true;
	Panel* m_parent = nullptr;
	std::vector<Panel*> m_children = std::vector<Panel*>();

};



#endif