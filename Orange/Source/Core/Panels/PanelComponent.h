#ifndef _PANEL_COMPONENT_H
#define _PANEL_COMPONENT_H

enum class PanelComponentType
{
	MAIN_VIEWPORT_PANEL,
	LOGGER,
	SPLINE_EDITOR,
	NONE
};

class Panel;

// Base class that Panels will collect in a vector,
// and overriding classes can define how to draw
// each component
class PanelComponent
{
public:

	virtual void Draw() = 0;

	PanelComponentType GetType();

protected:

	PanelComponentType m_type = PanelComponentType::NONE;
	Panel* m_parent = nullptr;

};

#endif
