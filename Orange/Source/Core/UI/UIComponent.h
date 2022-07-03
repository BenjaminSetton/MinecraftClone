#ifndef UICOMPONENT_H
#define UICOMPONENT_H

namespace Orange
{
	enum class UIComponentType
	{
		TEXTBOX = 0,
		IMAGE,
		SCROLLBAR
		// TODO - Add more
	};

	// This pure virtual class defines what a derived class has to implement to become
	// a well-defined, drawable UI element. It will also include some utility functions,
	// such as layer number.
	class UIComponent
	{
	public:

		virtual const UIComponentType GetType() const = 0;
		virtual void Draw() const = 0;

		// Public non-virtual functions
		const uint32_t GetLayerNumber() { return m_layerNumber; }
		void SetLayerNumber(const uint32_t layerNumber) { m_layerNumber = layerNumber; }

	protected:

		UIComponentType m_type;
		uint32_t m_layerNumber;

	};
}


#endif