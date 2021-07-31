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
enum EventCategory
{
	KEYBOARD = BIT(0),
	MOUSE = BIT(1),
	APPLICATION = BIT(2),
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


#endif