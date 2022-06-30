#ifndef _KEYBOARD_EVENT_H
#define _KEYBOARD_EVENT_H

#include "Event.h"

class KeyboardEvent : public Event
{
public:

	KeyboardEvent(const uint16_t k) : key(k) {}
	KeyboardEvent(const KeyboardEvent& other) = default;
	~KeyboardEvent() = default;

	virtual const EventType GetType() const = 0;
	virtual const EventCategory GetCategory() const = 0;

	const uint16_t GetKey() const { return key; }

private:

	const uint16_t key;
};

class KeyboardUpEvent : public KeyboardEvent
{
public:

	KeyboardUpEvent(const uint16_t k) : KeyboardEvent(k) {}
	KeyboardUpEvent(const KeyboardUpEvent& other) = default;
	~KeyboardUpEvent() = default;

	virtual const EventType GetType() const override { return EventType::KEYBOARD_KEY_UP_T; }
	virtual const EventCategory GetCategory() const override { return EventCategory::KEYBOARD; }

private:

	// No extra data is necessary
};

class KeyboardDownEvent : public KeyboardEvent
{
public:

	KeyboardDownEvent(const uint16_t k) : KeyboardEvent(k) {}
	KeyboardDownEvent(const KeyboardDownEvent& other) = default;
	~KeyboardDownEvent() = default;

	virtual const EventType GetType() const override { return EventType::KEYBOARD_KEY_DOWN_T; }
	virtual const EventCategory GetCategory() const override { return EventCategory::KEYBOARD; }

private:

	// No extra data is necessary
};

#endif