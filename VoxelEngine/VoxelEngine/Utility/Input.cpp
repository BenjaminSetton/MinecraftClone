#include "../Misc/pch.h"

#include "Input.h"

#include "../Core/Events/KeyboardEvent.h"

std::bitset<256> Input::m_keys = std::bitset<256>(false);

Input::Input() : EventObserver((EventCategory)((uint16_t)EventCategory::KEYBOARD | (uint16_t)EventCategory::MOUSE)){}
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

	switch(type)
	{

	case EventType::KEYBOARD_KEY_DOWN_T:
	{
		KeyboardDownEvent keyDown = (KeyboardDownEvent&)event;
		KeyDown(keyDown.GetKey());
	}
	case EventType::KEYBOARD_KEY_UP_T:
	{
		KeyboardUpEvent keyUp = (KeyboardUpEvent&)event;
		KeyUp(keyUp.GetKey());
	}
	case EventType::MOUSE_BUTTON_DOWN_T:
	{

	}
	case EventType::MOUSE_BUTTON_UP_T:
	{

	}
	case EventType::MOUSE_MOVED_T:
	{

	}
	case EventType::MOUSE_SCROLLED_T:
	{

	}

	}


}
