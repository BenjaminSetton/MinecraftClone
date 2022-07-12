#ifndef _UIMANAGER_H
#define _UIMANAGER_H

#include "../../Utility/MathTypes.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace Orange
{
	// Forward declaration
	class UIComponent;

	class UIManager
	{
	public:

		struct NextComponentData
		{
			Vec2 pos;
			Vec2 size;
		};

	public:

		static void Initialize();
		static void Deinitialize();

		static void Draw();


		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// 
		// The following functions will populate the m_components map and their hashes. These can be called every frame
		// in order to update the existing instance of UIComponent. If they are not called every frame that
		// will not cause the object to be removed from the vector, and it will still be rendered. Removing
		// will only happen if it's explicit, such as calling the respective Remove() function
		//
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		static void Text(const std::string& text);
		static void Image(const void* data, Vec2 size);

		static void PushData(const NextComponentData& data);
		static void PopData();

	private:

		static std::vector<UIComponent*> m_componentList;
		static std::unordered_map<uint64_t, UIComponent*> m_componentHashMap;

		static NextComponentData m_nextData;
		static bool m_nextDataValid;

	};
}

#endif