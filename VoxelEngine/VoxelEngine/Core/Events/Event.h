#ifndef _EVENT_H
#define _EVENT_H

#include "../../Utility/Utility.h"

// Event type enumerations
enum class EventType
{
	NONE = 0,
	KEYBOARD_KEY_DOWN_T, KEYBOARD_KEY_UP_T,
	MOUSE_BUTTON_DOWN_T, MOUSE_BUTTON_UP_T, MOUSE_MOVED_T, MOUSE_SCROLLED_T
};

// Event category enumerations
enum class EventCategory
{
	UNDEFINED		= BIT(0),
	KEYBOARD		= BIT(1),
	MOUSE			= BIT(2),
	APPLICATION		= BIT(3),
};

class Event
{
public:

	Event() = default;
	Event(const Event& other) = default;
	~Event() = default;

	virtual const EventType GetType() const = 0;
	virtual const EventCategory GetCategory() const = 0;

};

// Event data classes
class KeyboardEvent : public Event
{
public:
	// Rule of three + overloaded constructor
	KeyboardEvent() = default;
	KeyboardEvent(const EventType eventType) : Event(eventType) {}
	KeyboardEvent(const KeyboardEvent& other) = default;
	~KeyboardEvent() = default;

	// Event functions
	virtual const EventCategory GetCategory() const override { return EventCategory::KEYBOARD; }

	// Keyboard data
	unsigned int key;
	bool isPressed;
};


#endif