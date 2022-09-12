#ifndef _INPUT_H
#define _INPUT_H

#include <bitset>
#include "../Core/Events/IEvent.h"
#include "../Core/Events/KeyCodes.h"
#include "../Utility/MathTypes.h"

namespace Orange
{
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
		static bool IsMouseClicked(const uint16_t button);
		static void MouseMoved(const float x, const float y);
		static const Vec2 GetMouseDelta();
		static const Vec2 GetMouseDeltaRelativeToBottomLeft();
		static const Vec2 GetMousePosition();
		static const Vec2 GetMousePositionRelativeToBottomLeft();

		// Scroll wheel methods
		static void SetScroll(const float dist);
		static const float GetScroll();

		virtual void OnEvent(const Event& event) override;

		static void Update();

	private:

		// Keyboard
		static std::bitset<256> m_keys;

		// Mouse
		static std::bitset<16> m_mouseButtonsHeld;
		static std::bitset<16> m_mouseButtonsHeldPreviousFrame;
		static float m_scroll;
		static Vec2 m_prev;
		static Vec2 m_delta;
		static Vec2 m_mousePosition;

		static bool m_scrollProcessed;
		static bool m_ignoreFirstFrameForMouseDelta;
		static bool m_mouseMoved;
		static bool m_mouseClicked;
	};
}

#endif