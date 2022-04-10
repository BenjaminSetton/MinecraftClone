#ifndef _PANEL_H
#define _PANEL_H

#include <string>
#include <vector>

#include "../../imgui/imgui.h"
#include "../MathTypes.h"
#include "../../Utility/Utility.h"

enum PanelPropertyFlags_
{
	PanelPropertyFlags_NoResize		= ImGuiWindowFlags_NoResize,
	PanelPropertyFlags_NoMove		= ImGuiWindowFlags_NoMove,
	PanelPropertyFlags_NoDock		= ImGuiWindowFlags_NoDocking,
	PanelPropertyFlags_NoCollapse	= ImGuiWindowFlags_NoCollapse,
	PanelPropertyFlags_NoTitleBar	= ImGuiWindowFlags_NoTitleBar
};

// This enum value is usually paired with either a single float or two floats
enum PanelStyleFlags_
{
	PanelStyleFlags_WindowPadding = ImGuiStyleVar_WindowPadding,
	PanelStyleFlags_FramePadding = ImGuiStyleVar_FramePadding
};

typedef uint32_t PanelPropertyFlags;
typedef uint32_t PanelStyleFlags;

class Panel
{
public:

	Panel() = default;
	Panel(const Panel& panel) = default;
	virtual ~Panel() { }

	void Create(const std::string& name, const Orange::Vec2 dimensions, const Orange::Vec2 pos, const PanelPropertyFlags flags);
	void Shutdown();
	void Render();

	const std::string& GetName() const;
	Orange::Vec2 GetDimensions() const;
	Orange::Vec2 GetPos() const;
	PanelPropertyFlags GetPropertyFlags() const;

protected:

	virtual void Draw() = 0;

	std::string m_name = "";
	Orange::Vec2 m_dimensions = Orange::Vec2(0, 0);
	Orange::Vec2 m_pos = Orange::Vec2(0, 0);
	PanelPropertyFlags m_flags = 0;
	bool m_isOpen = true;

};



#endif