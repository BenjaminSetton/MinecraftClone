
#include <queue>

#include "TextBox.h"
#include "UIManager.h"
#include "../../Utility/Utility.h"

namespace Orange
{
	// Actual container of UIComponents
	std::vector<UIComponent*> UIManager::m_componentList = std::vector<UIComponent*>();

	// An unordered_map that maps from a UIComponent hash to it's actual pointer
	std::unordered_map<uint64_t, UIComponent*> UIManager::m_componentHashMap = std::unordered_map<uint64_t, UIComponent*>();

	UIManager::NextComponentData UIManager::m_nextData = { Vec2(0.0f, 0.0f), Vec2(0.0f, 0.0f)};
	bool UIManager::m_nextDataValid = false;

	void UIManager::Initialize()
	{
		// TODO
	}

	void UIManager::Deinitialize()
	{
		// TODO
	}

	void UIManager::Draw()
	{
		// A priority queue that sorts from lowest layer number to highest; this is used to draw the components in order
		std::priority_queue<UIComponent*> componentPQ;

		// Organizes all the components from lowest layer number to highest
		for(uint32_t i = 0; i < m_componentList.size(); i++ )
		{
			componentPQ.push(m_componentList[i]);
		}

		// We can then pop the priority queue to draw all the element in order. We also know that the PQ
		// will have the same size as our component list, so we can use a for loop and avoid the while loop
		for (uint32_t i = 0; i < m_componentList.size(); i++)
		{
			UIComponent* component = componentPQ.top();
			component->Draw();
			componentPQ.pop();
		}
	}

	void UIManager::Text(const std::string& text)
	{
		uint64_t hash = TextBox::CalculateHashFromText(text);
		auto element = m_componentHashMap.find(hash);
		if (element != m_componentHashMap.end()) // We found a matching hash
		{
			if (element->second->GetType() != UIComponentType::TEXTBOX)
			{
				OG_ASSERT_MSG(false, "We have a hash collision with another object (we might have hash collisions between two TextBoxes as well!");
			}
		}
		else // No matching hash
		{
			// Add new element to hash map
			TextBox* textBox = nullptr;
			if (m_nextDataValid)
			{
				textBox = new TextBox(text, m_nextData.pos, m_nextData.size);
			}
			else
			{
				textBox = new TextBox(text);
			}

			m_componentList.emplace_back(textBox);
			m_componentHashMap[hash] = m_componentList.back();
		}
	}

	void UIManager::Image(const void* data, Vec2 size)
	{
		UNUSED(data);
		UNUSED(size);
	}


	void UIManager::PushData(const NextComponentData& data)
	{
		// Sanity
		OG_ASSERT_MSG(m_nextDataValid == false, "Calling Push() consecutively without a Pop() in between!");

		m_nextData = data;
		m_nextDataValid = true;
	}

	void UIManager::PopData()
	{
		// Sanity
		OG_ASSERT_MSG(m_nextDataValid == true, "Calling Pop() consecutively without a Push() in between!");

		m_nextDataValid = false;
	}

}


