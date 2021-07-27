#ifndef _EVENT_H
#define _EVENT_H

enum class EventType
{
	None = 0,
	KeyboardKeyDown, KeyboardKeyUp,
	MouseLMBDown, MouseLMBUp, MouseRMBDown, MouseRMBUp
};

enum EventCategory
{
	Keyboard = 0,
	Mouse,
};

class Event
{
public:

private:

};

#endif