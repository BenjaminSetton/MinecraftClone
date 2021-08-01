#ifndef _INPUT_H
#define _INPUT_H

#include <bitset>
#include "../Core/Events/IEvent.h"

class Input : public EventObserver
{
public:

	Input();
	~Input() = default;
	Input(const Input&) = default;

	// Keyboard methods
	static void KeyUp(const uint16_t key);
	static void KeyDown(const uint16_t key);
	static bool IsKeyDown(const uint16_t key);

	// Mouse methods
	static void MouseDown(const uint16_t button);
	static void MouseUp(const uint16_t button);
	static bool IsMouseDown(const uint16_t button);
	static void MouseMoved(const float x, const float y);
	static const float GetMouseDeltaX();
	static const float GetMouseDeltaY();

	// Scroll wheel methods
	static void SetScroll(const float dist);
	static const float GetScroll();

	virtual void OnEvent(const Event& event) override;

	static void Update();

private:

	// Keyboard
	static std::bitset<256> m_keys;

	// Mouse
	static std::bitset<16> m_mouseButtons;

	// Scroll wheel
	static float m_scroll;

	// delta mouse position
	static float m_prevX, m_prevY;
	static float m_deltaX, m_deltaY;

	static bool m_scrollProcessed;
	static bool m_ignoreFirstFrameForMouseDelta;
	static bool m_mouseMoved;
};

#endif