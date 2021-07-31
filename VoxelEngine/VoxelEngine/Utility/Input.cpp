#include "../Misc/pch.h"

#include "Input.h"

#include "../Core/Events/KeyboardEvent.h"
#include "../Core/Events/MouseEvent.h"
#include "../Core/Events/KeyCodes.h"

std::bitset<256> Input::m_keys = std::bitset<256>(false);
std::bitset<16> Input::m_mouseButtons = std::bitset<16>(false);
float Input::m_scroll = 0;
float Input::m_deltaX = 0;
float Input::m_deltaY = 0;
float Input::m_prevX = 0;
float Input::m_prevY = 0;


bool Input::m_scrollProcessed = false;
bool Input::m_ignoreFirstFrameForMouseDelta = true;

Input::Input() : EventObserver((EventCategory)((uint16_t)EventCategory::KEYBOARD | (uint16_t)EventCategory::MOUSE)){}

void Input::KeyUp(const uint16_t key)
{
	m_keys[key] = false;
}
void Input::KeyDown(const uint16_t key)
{
	m_keys[key] = true;
}
bool Input::IsKeyDown(const uint16_t key)
{
	return m_keys[key];
}

void Input::MouseDown(const uint16_t button)
{
	m_mouseButtons[button] = true;
}

void Input::MouseUp(const uint16_t button)
{
	m_mouseButtons[button] = false;
}

bool Input::IsMouseDown(const uint16_t button)
{
	return m_mouseButtons[button];
}

void Input::MouseMoved(const float x, const float y)
{
	if (m_ignoreFirstFrameForMouseDelta)
	{
		m_ignoreFirstFrameForMouseDelta = false;
		m_prevX = x;
		m_prevY = y;
	}
	else
	{
		m_deltaX = x - m_prevX;
		m_deltaY = y - m_prevY;

		m_prevX = x;
		m_prevY = y;
	}
}

const float Input::GetMouseDeltaX()
{
	return m_deltaX;
}

const float Input::GetMouseDeltaY()
{
	return m_deltaY;
}

void Input::SetScroll(const float dist)
{
	m_scroll = dist;
	m_scrollProcessed = false;
}

const float Input::GetScroll()
{
	m_scrollProcessed = true;
	return m_scroll;
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
		return;
	}
	case EventType::KEYBOARD_KEY_UP_T:
	{
		KeyboardUpEvent keyUp = (KeyboardUpEvent&)event;
		KeyUp(keyUp.GetKey());
		return;
	}
	case EventType::MOUSE_BUTTON_DOWN_T:
	{
		MouseButtonDownEvent buttonDown = (MouseButtonDownEvent&)event;
		MouseDown(buttonDown.GetButton());
		return;

	}
	case EventType::MOUSE_BUTTON_UP_T:
	{
		MouseButtonUpEvent buttonUp = (MouseButtonUpEvent&)event;
		MouseUp(buttonUp.GetButton());
		return;
	}
	case EventType::MOUSE_MOVED_T:
	{
		MouseMovedEvent mouseMoved = (MouseMovedEvent&)event;
		MouseMoved(mouseMoved.GetX(), mouseMoved.GetY());
		return;
		
	}
	case EventType::MOUSE_SCROLLED_T:
	{
		MouseScrolledEvent mouseScrolled = (MouseScrolledEvent&)event;
		SetScroll(mouseScrolled.GetScrollDist());
		return;
	}

	}


}

// Update() is in charge of calculating the deltaX and deltaY for mouse positions
// and for resetting the scroll distance every frame
void Input::Update()
{
	// If the scroll has already been processed, set it to 0
	if (m_scrollProcessed) m_scroll = 0;



}
