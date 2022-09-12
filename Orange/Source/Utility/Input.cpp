#include "../Misc/pch.h"

#include "../Core/Application.h"
#include "Input.h"
#include "../Core/Events/KeyboardEvent.h"
#include "../Core/Events/MouseEvent.h"

namespace Orange
{
	std::bitset<256> Input::m_keys = std::bitset<256>(false);
	std::bitset<16> Input::m_mouseButtonsHeld = std::bitset<16>(false);
	std::bitset<16> Input::m_mouseButtonsHeldPreviousFrame = std::bitset<16>(false);
	float Input::m_scroll = 0;
	Vec2 Input::m_delta = { 0.0f, 0.0f };
	Vec2 Input::m_prev = { 0.0f, 0.0f };

	bool Input::m_scrollProcessed = false;
	bool Input::m_ignoreFirstFrameForMouseDelta = true;
	bool Input::m_mouseMoved = false;
	bool Input::m_mouseClicked = false;
	Orange::Vec2 Input::m_mousePosition = { 0.0f, 0.0f };

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
		m_mouseButtonsHeld[button] = true;
	}

	void Input::MouseUp(const uint16_t button)
	{
		m_mouseButtonsHeld[button] = false;
	}

	bool Input::IsMouseDown(const uint16_t button)
	{
		return m_mouseButtonsHeld[button];
	}

	bool Input::IsMouseClicked(const uint16_t button)
	{
		return m_mouseClicked;
	}

	void Input::MouseMoved(const float x, const float y)
	{

		if (m_ignoreFirstFrameForMouseDelta)
		{
			m_ignoreFirstFrameForMouseDelta = false;
		}
		else
		{
			m_delta = { x - m_prev.x, y - m_prev.y };
		}

		m_prev = { x, y };

		m_mousePosition = { x, y };

		m_mouseMoved = true;
	}

	const Vec2 Input::GetMouseDelta()
	{
		return m_delta;
	}

	const Vec2 Input::GetMouseDeltaRelativeToBottomLeft()
	{
		return Vec2(m_delta.x, -m_delta.y);
	}

	const Vec2 Input::GetMousePosition()
	{
		return m_mousePosition;
	}

	const Vec2 Input::GetMousePositionRelativeToBottomLeft()
	{
		Vec2 windowSize = Application::Handle->GetMainWindow()->GetSize();
		return Vec2(m_mousePosition.x, windowSize.y - m_mousePosition.y);
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
		m_mouseClicked = !m_mouseButtonsHeldPreviousFrame[MouseCode::LBUTTON] && m_mouseButtonsHeld[MouseCode::LBUTTON];
		
		// If the scroll has already been processed, set it to 0
		if (m_scrollProcessed) m_scroll = 0;

		if(!m_mouseMoved)
		{
			m_delta = { 0.0f, 0.0f };
		}

		// Set the mouse moved to false to reset mouseMoved messages
		m_mouseMoved = false;

		m_mouseButtonsHeldPreviousFrame = m_mouseButtonsHeld;
	}
}
