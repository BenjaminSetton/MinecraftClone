#ifndef _EVENT_H
#define _EVENT_H

#include "../../Utility/Utility.h"

// Event type enumerations
enum class EventType
{
	NONE = 0,
	KEYBOARD_KEY_DOWN, KEYBOARD_KEY_UP,
	MOUSE_BUTTON_DOWN, MOUSE_BUTTON_UP
};

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
	Event(const EventType eventType);
	Event(const Event& other) = default;
	~Event() = default;

	const EventType GetType() const;
	virtual const EventCategory GetCategory() const = 0;

protected:

	EventType m_type;

};

// Event data classes
class KeyboardEvent : public Event
{
public:
	// Rule of three + overloaded constructor
	KeyboardEvent() = default;
	KeyboardEvent(const EventType eventType);
	KeyboardEvent(const KeyboardEvent& other) = default;
	~KeyboardEvent() = default;

	// Event functions
	virtual const EventCategory GetCategory() const override;

	// Keyboard data
	unsigned int key;
	bool isPressed;
};

class MouseEvent : public Event
{
public:
	// Rule of three + overloaded constructor
	MouseEvent() = default;
	MouseEvent(const EventType eventType);
	MouseEvent(const MouseEvent & other) = default;
	~MouseEvent() = default;

	// Event functions
	virtual const EventCategory GetCategory() const override;

	// Mouse data
	float x, y;
	bool LMBDown, MMBDown, RMBDown;
};


#endif