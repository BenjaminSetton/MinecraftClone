#include "../Misc/pch.h"

#include "Input.h"
#include "../Core/ApplicationHandle.h"

std::bitset<256> Input::m_keys = std::bitset<256>(false);

Input::Input() : EventObserver(EventCategory::KEYBOARD) 
{
	ApplicationHandle->Subscribe(this);
}
Input::~Input(){}


void Input::KeyUp(unsigned int key)
{
	m_keys[key] = false;
}
void Input::KeyDown(unsigned int key) 
{
	m_keys[key] = true;
}
bool Input::IsKeyDown(unsigned int key)
{
	return m_keys[key];
}

void Input::OnEvent(const Event& event)
{
	EventCategory category = event.GetCategory();
	VX_ASSERT(category == EventCategory::KEYBOARD);

	KeyboardEvent keyData = (KeyboardEvent&)event;
	
	if (keyData.isPressed) KeyDown(keyData.key);
	else KeyUp(keyData.key);

	VX_LOG_INFO("OnEvent is called");
}
