#ifndef UICOMPONENT_H
#define UICOMPONENT_H

namespace Orange
{
	enum class UIComponentType
	{
		INVALID = 0,
		TEXTBOX,
		IMAGE,
		SCROLLBAR
		// TODO - Add more
	};

	static Vec2 QUAD_COORDINATES[6] =
	{
		Vec2(1, 1), // BR
		Vec2(1, 0), // TR
		Vec2(0, 0), // TL
		Vec2(0, 1), // BL
		Vec2(1, 1), // BR
		Vec2(0, 0), // TL
	};

	// We define a vertex with a single coordinate that combines both the position and UV coordinate. When we pass the size and position
	// of the quad to the vertex shader, we can extract the position by multiplying by the size and adding the position (since the 'coordinate'
	// variable is normalized). The UV coordinate should be exactly the same, but this means that the vertices that we pass in are extremely light-weight
	struct UIVertex
	{
		// Per-vertex data
		Vec2 coordinate = Vec2(0, 0);

		// Per-instance data
		Vec2 size = Vec2(0, 0);
		Vec2 position = Vec2(0, 0);
	};

	// This virtual class defines what a derived class has to implement to become
	// a well-defined, drawable UI element. It will also include some utility functions,
	// such as layer number.
	class UIComponent
	{
	public:

		virtual const UIComponentType GetType() const { return UIComponentType::INVALID; }
		virtual void Draw() const { return; }
		// Every UIComponent will have to calculate a hash using one of it's member variables as
		// parameters. This is because the UIManager will call this function when a new UI element
		// is being added so that it's not added to the internal buffer more than once. Please make
		// sure that this hash is consistent between function calls. It will have to be the same unless
		// some important member variable is changed (eg the text for a TextBox object)
		virtual const uint64_t CalculateHash() { return 0; };

		const Vec2 GetPosition() const { return m_position; };
		const Vec2 GetSize() const { return m_size; };
		const Vec2 GetPadding() const { return m_padding; };

		// Public non-virtual functions
		const uint32_t GetLayerNumber() const { return m_layerNumber; }
		void SetLayerNumber(const uint32_t layerNumber) { m_layerNumber = layerNumber; }

		// This function is overridden so that the UIManager can use a priority queue to sort
		// the elements by layer and draw them!
		bool operator<(const UIComponent& other)
		{
			return GetLayerNumber() < other.GetLayerNumber();
		}

	protected:

		void SetPosition(const Vec2 position) { m_position = position; };
		void SetSize(const Vec2 size) { m_size = size; };
		void SetPadding(const Vec2 padding) { m_padding = padding; };

		Vec2 m_position;
		Vec2 m_size;
		Vec2 m_padding;
		uint32_t m_layerNumber;

	};
}


#endif