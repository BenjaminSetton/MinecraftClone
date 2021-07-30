#include "../Misc/pch.h"

#include "Input.h"

#include "../Core/Events/KeyboardEvent.h"

std::bitset<256> Input::m_keys = std::bitset<256>(false);

Input::Input() : EventObserver(EventCategory::KEYBOARD){}
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
	auto type = event.GetType();

	if (type == EventType::KEYBOARD_KEY_DOWN_T)
	{
		KeyboardDownEvent keyDown = (KeyboardDownEvent&)event;
		KeyDown(keyDown.GetKey());
		VX_LOG_INFO(static_cast<char>(keyDown.GetKey()));
	}
	else if (type == EventType::KEYBOARD_KEY_UP_T)
	{
		KeyboardUpEvent keyUp = (KeyboardUpEvent&)event;
		KeyUp(keyUp.GetKey());
	}

}
